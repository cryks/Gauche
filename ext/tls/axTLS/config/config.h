/*
 * In original axTLS, this file is automatically generated.
 * To include in Gauche, we hand-edited this file, so be careful
 * not to clobber this file.
 */

/*
 * General Configuration
 */
#define CONFIG_DEBUG 1

/*
 * SSL Library
 */
#undef CONFIG_SSL_SERVER_ONLY
#undef CONFIG_SSL_CERT_VERIFICATION
#undef CONFIG_SSL_ENABLE_CLIENT
#define CONFIG_SSL_FULL_MODE 1
#undef CONFIG_SSL_SKELETON_MODE
#undef CONFIG_SSL_PROT_LOW
#define CONFIG_SSL_PROT_MEDIUM 1
#undef CONFIG_SSL_PROT_HIGH
#define CONFIG_SSL_USE_DEFAULT_KEY 1
#define CONFIG_SSL_PRIVATE_KEY_LOCATION ""
#define CONFIG_SSL_PRIVATE_KEY_PASSWORD ""
#define CONFIG_SSL_X509_CERT_LOCATION ""
#undef CONFIG_SSL_GENERATE_X509_CERT
#define CONFIG_SSL_X509_COMMON_NAME ""
#define CONFIG_SSL_X509_ORGANIZATION_NAME ""
#define CONFIG_SSL_X509_ORGANIZATION_UNIT_NAME ""
#undef CONFIG_SSL_ENABLE_V23_HANDSHAKE
#define CONFIG_SSL_HAS_PEM 1
#define CONFIG_SSL_USE_PKCS12 1
#define CONFIG_SSL_EXPIRY_TIME 24
#define CONFIG_X509_MAX_CA_CERTS 150
#define CONFIG_SSL_MAX_CERTS 3
#undef CONFIG_SSL_CTX_MUTEXING
#define CONFIG_USE_DEV_URANDOM 1
#ifdef WIN32
#define CONFIG_WIN32_USE_CRYPTO_LIB 1
#endif /*WIN32*/
#undef CONFIG_OPENSSL_COMPATIBLE
#undef CONFIG_PERFORMANCE_TESTING
#undef CONFIG_SSL_TEST
#undef CONFIG_AXTLSWRAP
#undef CONFIG_AXHTTPD
#undef CONFIG_HTTP_STATIC_BUILD
#define CONFIG_HTTP_PORT 
#define CONFIG_HTTP_HTTPS_PORT 
#define CONFIG_HTTP_SESSION_CACHE_SIZE 
#define CONFIG_HTTP_WEBROOT ""
#define CONFIG_HTTP_TIMEOUT 
#undef CONFIG_HTTP_HAS_CGI
#define CONFIG_HTTP_CGI_EXTENSIONS ""
#undef CONFIG_HTTP_ENABLE_LUA
#define CONFIG_HTTP_LUA_PREFIX ""
#define CONFIG_HTTP_LUA_CGI_LAUNCHER ""
#undef CONFIG_HTTP_BUILD_LUA
#undef CONFIG_HTTP_DIRECTORIES
#undef CONFIG_HTTP_HAS_AUTHORIZATION
#undef CONFIG_HTTP_HAS_IPV6
#undef CONFIG_HTTP_ENABLE_DIFFERENT_USER
#define CONFIG_HTTP_USER ""
#undef CONFIG_HTTP_VERBOSE
#undef CONFIG_HTTP_IS_DAEMON

/*
 * Language Bindings
 */
#undef CONFIG_BINDINGS
#undef CONFIG_CSHARP_BINDINGS
#undef CONFIG_VBNET_BINDINGS
#define CONFIG_DOT_NET_FRAMEWORK_BASE ""
#undef CONFIG_JAVA_BINDINGS
#define CONFIG_JAVA_HOME ""
#undef CONFIG_PERL_BINDINGS
#define CONFIG_PERL_CORE ""
#define CONFIG_PERL_LIB ""
#undef CONFIG_LUA_BINDINGS
#define CONFIG_LUA_CORE ""

/*
 * Samples
 */
#define CONFIG_SAMPLES 1
#define CONFIG_C_SAMPLES 1
#undef CONFIG_CSHARP_SAMPLES
#undef CONFIG_VBNET_SAMPLES
#undef CONFIG_JAVA_SAMPLES
#undef CONFIG_PERL_SAMPLES
#undef CONFIG_LUA_SAMPLES

/*
 * BigInt Options
 */
#undef CONFIG_BIGINT_CLASSICAL
#undef CONFIG_BIGINT_MONTGOMERY
#define CONFIG_BIGINT_BARRETT 1
#define CONFIG_BIGINT_CRT 1
#undef CONFIG_BIGINT_KARATSUBA
#define MUL_KARATSUBA_THRESH 
#define SQU_KARATSUBA_THRESH 
#define CONFIG_BIGINT_SLIDING_WINDOW 1
#define CONFIG_BIGINT_SQUARE 1
#define CONFIG_BIGINT_CHECK_ON 1
#define CONFIG_INTEGER_32BIT 1
#undef CONFIG_INTEGER_16BIT
#undef CONFIG_INTEGER_8BIT

/* The following macros rename APIs in crypto/*.  This is to avoid build-time
   problems when those names conflict with system-provided ones. */
#define AES_set_key       AES_set_key__axtls
#define AES_cbc_encrypt   AES_cbc_encrypt__axtls
#define AES_cbc_decrypt   AES_cbc_decrypt__axtls
#define AES_convert_key   AES_convert_key__axtls
#define RC4_setup         RC4_setup__axtls
#define RC4_crypt         RC4_crypt__axtls
#define SHA1_Init         SHA1_Init__axtls
#define SHA1_Update       SHA1_Update__axtls
#define SHA1_Final        SHA1_Final__axtls
#define MD2_Init          MD2_Init__axtls
#define MD2_Update        MD2_Update__axtls
#define MD2_final         MD2_Final__axtls
#define MD5_Init          MD5_Init__axtls
#define MD5_Update        MD5_Update__axtls
#define MD5_Final         MD5_Final__axtls
#define hmac_md5          hmac_md5__axtls
#define hmac_sha1         hmac_sha1__axtls
#define RSA_priv_key_new  RSA_priv_key_new__axtls
#define RSA_pub_key_new   RSA_pub_key_new__axtls
#define RSA_free          RSA_free__axtls
#define RSA_decrypt       RSA_decrypt__axtls
#define RSA_private       RSA_private__axtls
#define RSA_sign_verify   RSA_sign_verify__axtls
#define RSA_public        RSA_public__axtls
#define RSA_encrypt       RSA_encrypt__axtls
#define RSA_print         RSA_print__axtls
#define RNG_initialize    RNG_initialize__axtls
#define RNG_terminate     RNG_terminate__axtls
#define get_random        get_random__axtls
#define get_random_NZ     get_random_NZ__axtls
