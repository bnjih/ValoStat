#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <unistd.h>

#define API_BASE_URL "https://valorant-api.com/v1/"
//langues disponibles
// ar-AE / de-DE / en-US / es-ES / es-MX / fr-FR / id-ID / it-IT / ja-JP / ko-KR / pl-PL / pt-BR / ru-RU / th-TH / tr-TR / vi-VN / zh-CN / zh-TW
#define API_LANGUAGE "?language=fr-FR"

struct MemoryStruct {
    char *memory;
    size_t size;
};

// Fonction de rappel pour reçevoir une réponse de l'API
static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realSize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realSize + 1);
    if (ptr == NULL) {
        printf("Erreur lors de l'allocation de mémoire.\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realSize);
    mem->size += realSize;
    mem->memory[mem->size] = '\0';

    return realSize;
}

//Fonction pour faire une demande de l'API et retrouver les datas JSON
char *makeAPIRequest(const char *endpoint) {
    CURL *curl = curl_easy_init();
    char *response = NULL;
    struct MemoryStruct chunk;

    if (curl) {
        chunk.memory = malloc(1);
        chunk.size = 0;

        char url[256];
        snprintf(url, sizeof(url), "%s%s%s", API_BASE_URL, endpoint, API_LANGUAGE);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() a raté: %s\n", curl_easy_strerror(res));
        }

        response = chunk.memory;

        curl_easy_cleanup(curl);
    }

    return response;
}

// Fonction pour retrouver et procéder les agents
void getAgents() {
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "agents");

    char *response = makeAPIRequest(endpoint);
    if (response) {
        json_error_t error;
        json_t *root = json_loads(response, 0, &error);
        if (!root) {
            fprintf(stderr, "Erreur lors du parse JSON: %s\n", error.text);
        } else {
            json_t *data = json_object_get(root, "data");
            size_t agentCount = json_array_size(data);
            if (agentCount > 0) {
                json_t *agent = NULL;
                for (size_t i = 0; i < agentCount; i++) {
                    json_t *currentAgent = json_array_get(data, i);
                    json_t *displayName = json_object_get(currentAgent, "displayName");
                    const char *displayNameStr = json_string_value(displayName);
                    printf("Nom de l'agent: %s\n", displayNameStr);
                }
            } else {
                printf("Pas d'agents trouvés.\n");
            }

            json_decref(root);
        }
        free(response);
    }
}

// Fonction pour retrouver et procéder les agents par le nom
void getAgentDetails(const char *agentName) {
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "agents");

    char *response = makeAPIRequest(endpoint);
    if (response) {
        json_error_t error;
        json_t *root = json_loads(response, 0, &error);
        if (!root) {
            fprintf(stderr, "Erreur lors du parse JSON: %s\n", error.text);
        } else {
            json_t *data = json_object_get(root, "data");
            size_t agentCount = json_array_size(data);
            if (agentCount > 0) {
                json_t *agent = NULL;
                for (size_t i = 0; i < agentCount; i++) {
                    json_t *currentAgent = json_array_get(data, i);
                    json_t *displayName = json_object_get(currentAgent, "displayName");
                    const char *displayNameStr = json_string_value(displayName);
                    if (strcmp(displayNameStr, agentName) == 0) {
                        agent = currentAgent;
                        break;
                    }
                }
                if (agent != NULL) {
                    json_t *name = json_object_get(agent, "displayName");
                    json_t *description = json_object_get(agent, "description");

                    const char *nameStr = json_string_value(name);
                    const char *descriptionStr = json_string_value(description);

                    printf("Nom de l'agent: %s\n", nameStr);
                    printf("Description de l'agent: %s\n", descriptionStr);
                } else {
                    printf("Agent pas trouvé.\n");
                }
            } else {
                printf("Pas d'agents trouvés.\n");
            }

            json_decref(root);
        }
        free(response);
    }
}

// Fonction pour retrouver et procéder les détails de la map par le nom
void getMapDetails(const char *mapName) {
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "maps");

    char *response = makeAPIRequest(endpoint);
    if (response) {
        json_error_t error;
        json_t *root = json_loads(response, 0, &error);
        if (!root) {
            fprintf(stderr, "Erreur lors du parse JSON: %s\n", error.text);
        } else {
            json_t *data = json_object_get(root, "data");
            size_t mapCount = json_array_size(data);
            if (mapCount > 0) {
                json_t *map = NULL;
                for (size_t i = 0; i < mapCount; i++) {
                    json_t *currentMap = json_array_get(data, i);
                    json_t *displayName = json_object_get(currentMap, "displayName");
                    const char *displayNameStr = json_string_value(displayName);
                    if (strcmp(displayNameStr, mapName) == 0) {
                        map = currentMap;
                        break;
                    }
                }
                if (map != NULL) {
                    json_t *name = json_object_get(map, "displayName");
                    json_t *icon = json_object_get(map, "displayIcon");

                    const char *nameStr = json_string_value(name);
                    const char *iconStr = json_string_value(icon);

                    printf("Nom de la map: %s\n", nameStr);
                    printf("Lien vers l'icone de la map: %s\n", iconStr);
                } else {
                    printf("Map pas trouvé.\n");
                }
            } else {
                printf("Map pas trouvé.\n");
            }

            json_decref(root);
        }
        free(response);
    }
}

// Fonction pour retrouver et procéder les maps
void getMaps() {
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "maps");

    char *response = makeAPIRequest(endpoint);
    if (response) {
        json_error_t error;
        json_t *root = json_loads(response, 0, &error);
        if (!root) {
            fprintf(stderr, "Erreur lors du parse JSON: %s\n", error.text);
        } else {
            json_t *data = json_object_get(root, "data");
            size_t mapCount = json_array_size(data);
            if (mapCount > 0) {
                json_t *map = NULL;
                for (size_t i = 0; i < mapCount; i++) {
                    json_t *currentMap = json_array_get(data, i);
                    json_t *displayName = json_object_get(currentMap, "displayName");
                    const char *displayNameStr = json_string_value(displayName);
                    printf("Nom de la map: %s\n", displayNameStr);
                }
            } else {
                printf("Pas de maps trouvées.\n");
            }

            json_decref(root);
        }
        free(response);
    }
}

// Fonction main
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Utilisation de valostat:\n./valostat liste maps/agents\n./valostat agents/maps 'nom de l'agent/map'\n");
        return 1;
    }

    // Création d'un pipe pour une communication inter-opérations
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    // Création de l'operation fils
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Operation fils

        // Fermer la fin de lecture du pipe
        close(fd[0]);

        // Rediriger la sortie standard vers le pipe
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            return 1;
        }

        // Execution de la bonne commande selon ce que le client tape
        if (strcmp(argv[1], "liste") == 0 && strcmp(argv[2], "maps") == 0) {
            // Utilise le API de liste de map
            getMaps();
        } else if (strcmp(argv[1], "liste") == 0 && strcmp(argv[2], "agents") == 0) {
            // Utilise le API de liste de agents
            getAgents();
        } else if (strcmp(argv[1], "agents") == 0 && argc >= 3) {
            // Recevoir les données de l'agent avec le nom
            getAgentDetails(argv[2]);
        } else if (strcmp(argv[1], "maps") == 0 && argc >= 3) {
            // Recevor les données de la map avec le nom
            getMapDetails(argv[2]);
        } else {
            printf("Un des arguments n'est pas valide, veuillez réessayer.\n");
            return 1;
        }

        // Fermer l'écriture de fin du pipe
        close(fd[1]);
    } else {
        // Operation parent

        // Fermer l'écriture de fin du pipe
        close(fd[1]);

        // Attente pour l'opération du fils pour qu'elle se finalise
        wait(NULL);

        // Lecture de la réponse avec le pipe
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytesRead);
        }

        // Fermeture du pipe
        close(fd[0]);
    }

    return 0;
}
