;;;
;;; dbi - common database interface layer
;;;
;;;  Copyright (c) 2003-2005 Scheme Arts, L.L.C., All rights reserved.
;;;  Copyright (c) 2003-2005 Time Intermedia Corporation, All rights reserved.
;;;
;;;   Redistribution and use in source and binary forms, with or without
;;;   modification, are permitted provided that the following conditions
;;;   are met:
;;;   
;;;   1. Redistributions of source code must retain the above copyright
;;;      notice, this list of conditions and the following disclaimer.
;;;  
;;;   2. Redistributions in binary form must reproduce the above copyright
;;;      notice, this list of conditions and the following disclaimer in the
;;;      documentation and/or other materials provided with the distribution.
;;;  
;;;   3. Neither the name of the authors nor the names of its contributors
;;;      may be used to endorse or promote products derived from this
;;;      software without specific prior written permission.
;;;  
;;;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;;;   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;;;   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;;;   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;;;   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;;;   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
;;;   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
;;;   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
;;;   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
;;;   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;;;   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;;;
;;;  $Id: dbi.scm,v 1.7 2005-07-14 09:11:18 shirok Exp $
;;;

;;; *EXPERIMENTAL*
;;; This module provides an abstract interface to various database
;;; management systems.  

(define-module dbi
  (use text.sxql)
  (use file.util)
  (use srfi-1)
  (use srfi-13)
  (use util.match)
  (export <dbi-error> <dbi-nonexistent-driver-error>
          <dbi-unsupported-error> <dbi-parameter-error>
          <dbi-driver> <dbi-connection> <dbi-query> <dbi-result-set>
          dbi-connect dbi-close dbi-prepare dbi-execute dbi-do
          dbi-parse-dsn dbi-make-driver
          dbi-prepare-sql dbi-escape-sql dbi-list-drivers
          dbd-make-connection dbd-prepare dbd-execute
          ;; compatibility
          dbi-make-connection dbi-make-query dbi-execute-query dbi-get-value
          <dbi-exception>))
(select-module dbi)

;;;==============================================================
;;; DBI conditions
;;;

;; Root of dbi-related errors
(define-condition-type <dbi-error> <error> #f)

;; Failed to load the specified driver
(define-condition-type <dbi-nonexistent-driver-error> <dbi-error> #f
  ((driver-name :init-keyword :driver-name)))

;; Feature not supported
(define-condition-type <dbi-unsupported-error> <dbi-error> #f)

;; Parameter mismatch between a prepared query and its execution.
(define-condition-type <dbi-parameter-error> <dbi-error> #f)


;;;==============================================================
;;; DBI object definitions
;;;

;; <dbi-driver> is the base class of database drivers; a database
;; driver implements actual interface to a specific database system.
;; Each dbd.* module provides the concrete implementation.
;;
;; Usually the singleton instance of a concrete driver is created
;; implicitly by dbi-connect with the data-source string.  So the
;; user hardly need to see <dbi-driver> object.

(define-class <dbi-driver> ()
  ((driver-name :init-keyword :driver-name)  ; "mysql" "pg" etc.
   ))

;; Base of all dbi transient objects
(define-class <dbi-object> ()
  ((open   :getter dbi-open? :init-keyword :open :init-value #f)))

(define-method dbi-close ((o <dbi-object>))
  (set! (ref o 'open) #f))

;; <dbi-connection> : represents a connection to the database system.
;; All the transactions must be done while the connection is 'open'.
(define-class <dbi-connection> (<dbi-object>) ())

;; <dbi-query> : represents a query.  Query can be sent to the database
;; system by dbi-execute-query, to obtain a result set.
;; %prepared slot is used to store a prepared statemet by the DBI's default
;; SQL statement preparation method.  The driver may prepare statements
;; differently.
(define-class <dbi-query> (<dbi-object>)
  ((connection :init-value #f :init-keyword :connection)
   (%prepared  :init-value #f :init-keyword :%prepared)
  ))

;; <dbi-result-set> : an abstract entity of the result of a query.
;; For RDBMS, it is a set of rows.
(define-class <dbi-result-set> (<dbi-object>) ())

;;;==============================================================
;;; User-level APIs
;;;

;; Establish a connection to the data source specified by DSN,
;; and returns a connection object.
;; DSN is the data source name, which can have the following syntax.
;;   "dbi:driver-type"
;;   "dbi:driver-type:connection-options"
;; Connection-options is like "name1=value1;name2=value2;...".
;; 
(define (dbi-connect dsn . args)
  (receive (driver-name options option-alist) (dbi-parse-dsn dsn)
    (apply dbd-make-connection
           (dbi-make-driver driver-name) options option-alist args)))

;; Prepares and returns a query object.  The default method
;; parse SQL and store it in <dbi-query>.  The driver may overload
;; this method to delegate preparation in the DBMS.
(define-method dbi-prepare ((c <dbi-connection>) (sql <string>) . options)
  (let1 q (apply dbd-prepare c sql options)
    (set! (ref q 'connection) c)
    q))

;; Execute the prepared statement.
(define-method dbi-execute ((q <dbi-query>) . args)
  (apply dbd-execute (ref q 'connection) q args))

;; Does preparation and execution at once.  The driver may overload this.
(define-method dbi-do ((c <dbi-connection>) sql options . args)
  (apply dbd-execute (apply dbd-prepare c sql options) args))

(define-method dbi-do ((c <dbi-connection>) sql)
  (dbi-do c sql '()))

;; Returns a string safe to be embedded in SQL.
;;   (dbi-escape-sql c "Don't know") => "'Don''t know'"
;; What's "safe" depends on the underlying DBMS.  The default procedure
;; only escapes a single quote by repeating it.  If the DBMS has other
;; type of escaping mechanism, the driver should overload this with
;; a proper escaping method.
(define-method dbi-escape-sql ((c <dbi-connection>) str)
  (string-append "'" (regexp-replace-all #/'/ str "''") "'"))

;; Returns a list of available dbd.* backends.  Each entry is
;; a cons of a module name and its driver name.
(define (dbi-list-drivers)
  (library-map 'dbd.* (lambda (m p) m)))
  
;;;==============================================================
;;; DBD-level APIs
;;;

;; Subclass SHOULD implement this.
(define-method dbd-make-connection ((d <dbi-driver>) options option-alist
                                    . args)
  ;; The default method here is just a temporary one to use
  ;; older dbd drivers.  Will go away once the drivers catch up
  ;; the new interface.
  (let-keywords* args ((username "")
                       (password ""))
    ;; call deprecated dbi-make-connection API.
    (dbi-make-connection d username password (or options ""))))

;; Subclass may override this method.
(define-method dbd-prepare ((c <dbi-connection>) (sql <string>) . options)
  (make <dbi-query> :%prepared (dbi-prepare-sql c sql)))

;; Subclass should implement this.  The current default procedure
;; delegates the work for the old driver API.  Should go away soon.
(define-method dbd-execute ((c <dbi-connection>) (q <dbi-query>) . params)
  (or (and-let* ((prepared (slot-ref q '%prepared)))
        (dbi-execute-query (dbi-make-query c) (apply prepared params)))
      (error <dbi-unsupported-error> "dbi-execute is not implemented on" q)))
    
;; Subclass should implement this.
(define-method dbi-get-value ((r <dbi-result-set>) (n <integer>))
  '())

;;;===================================================================
;;; Low-level utilities
;;;

;; Parse data source name.  Returns 
;;  (driver-name, option-string, option-alist)
;;
(define (dbi-parse-dsn data-source-name)
  (rxmatch-case data-source-name
    (#/^dbi:([\w-]+)(?::(.+))?$/ (#f driver options)
     (if options
       (let1 alist (map (lambda (nv)
                          (receive (n v) (string-scan nv "=" 'both)
                            (if n (cons n v) (cons nv #t))))
                        (string-split options #\;))
         (values driver options alist))
       (values driver #f #f)))
    (else
     (error <dbi-error> "bad data source name spec:" data-source-name))))

;; Loads a concrete driver module, and returns an instance of
;; the driver.
(define (dbi-make-driver driver-name)
  (or (and-let* ((module&path (library-fold
                               (string->symbol #`"dbd.,driver-name")
                               (lambda (m p s) (cons m p)) #f))
                 (module      (car module&path))
                 (path        (cdr module&path))
                 (class-name  (string->symbol #`"<,|driver-name|-driver>"))
                 
                 (driver-class
                  (begin (eval `(require ,(path-sans-extension path))
                               (current-module))
                         (global-variable-ref module class-name #f)))
                 )
        (make driver-class :driver-name driver-name))
      (errorf <dbi-nonexistent-driver-error>
              :driver-name driver-name
              "couldn't load driver dbd.~a" driver-name)))

;; Default prepared-SQL handler
;; dbi-prepare-sql returns a procedure, which generates a complete sql
;; when called with binding values to the parameters.
(define (dbi-prepare-sql conn sql)
  (let* ((tokens (sql-tokenize sql))
         (num-params (count (lambda (elt)
                              (match elt
                                (('parameter (? integer?)) #t)
                                (('parameter (? string? name))
                                 (errorf <dbi-unsupported-error>
                                         "Named parameter (:~a) isn't supported yet" name))
                                (else #f)))
                            tokens)))
    (lambda args
      (unless (= (length args) num-params)
        (error <dbi-parameter-error>
               "wrong number of parameters given to an SQL:" sql))
      (call-with-output-string
        (lambda (p)
          (with-port-locking p
            (cut generate-sql/parameters conn tokens args p)))))))

(define (generate-sql/parameters conn tokens args p)
  (let loop ((tokens tokens)
             (args   args)
             (delim  #t))
    (unless (null? tokens)
      (match (car tokens)
        ((? symbol? x)
         (unless delim (write-char #\space p))
         (display x p)
         (loop (cdr tokens) args #f))
        ((? char? x)
         (display x p)
         (loop (cdr tokens) args #t))
        (('delimited x)
         (unless delim (write-char #\space p))
         (format p "\"~a\"" (regexp-replace-all #/\"/ x "\"\""))
         (loop (cdr tokens) args #f))
        (('string x)
         (unless delim (write-char #\space p))
         (format p "'~a'" (regexp-replace-all #/'/ x "''"))
         (loop (cdr tokens) args #f))
        (('number x)
         (unless delim (write-char #\space p))
         (display x)
         (loop (cdr tokens) args #f))
        (('parameter n)
         (unless delim (write-char #\space p))
         (display (dbi-escape-sql conn (car args)) p)
         (loop (cdr tokens) (cdr args) #f))
        (('bitstring x)
         (unless delim (write-char #\space p))
         (format p "B'~a'" x)
         (loop (cdr tokens) args #f))
        (('hexstring x)
         (unless delim (write-char #\space p))
         (format p "X'~a'" x)
         (loop (cdr tokens) args #f))
        (else
         (errorf <dbi-unsupported-error>
                 "unsupported SQL token ~a in ~s" (car tokens) sql))
        ))))


;;;==============================================================
;;; Backward compatibility stuff
;;;

;; These are provided for compatibility with dbi-0.1.5 and dbd-*
;; modules that depend on it.  The newly written code shouldn't use
;; these interface.  Will be gone in a few releases.

(define <dbi-exception> <dbi-error>)

;; Older API
(define-method dbi-make-connection ((d <dbi-driver>) user pass options)
  #f)
(define-method dbi-make-query ((c <dbi-connection>) (o <string>))
  #f)
(define-method dbi-make-query ((c <dbi-connection>))
  #f)
(define-method dbi-execute-query ((q <dbi-query>) (s <string>))
  #f)


(provide "dbi")