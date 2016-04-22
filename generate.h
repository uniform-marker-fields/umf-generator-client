
#define SERVER_URL "localhost"
#define GETMAP_PATH "/pmgen/getmap.php?v=2"
#define POSTMAP_PATH "/pmgen/postmap.php"

#define EXPERIMENT_FLAG "&test=on"

#define DEFAULT_TIMEOUT_SECONDS 120
#define DEFAULT_SLEEP_TIME_SECONDS 60
#define DEFAULT_SEND_TIME_SECONDS 60
#define DEFAULT_END_TIME 0.1

#define TRUE 1
#define FALSE 0

int download(const char *server, const char *path, std::string &data);
