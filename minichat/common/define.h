
#ifdef RELEASE

#define MINI_REDIS_CLIENT_CONFIG "~/mmminichat/etc/redis-client.conf"
#define MINI_PUB_KEY_PATH "~/mmminichat/etc/minichat_pubkey.pem"
#define MINI_PRIV_KEY_PATH "~/mmminichat/etc/minichat_privkey.pem"

#else

#define MINI_REDIS_CLIENT_CONFIG "~/minichat/etc/client/redis_client.conf"
#define MINI_PUB_KEY_PATH "~/minichat/etc/client/minichat_pubkey.pem"
#define MINI_PRIV_KEY_PATH "~/minichat/etc/minichat_privkey.pem"

#endif

