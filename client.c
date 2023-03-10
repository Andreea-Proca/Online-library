#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

char* reg_log_payload(char username[50], char password[50], int *ok) {
    JSON_Value *json_val = json_value_init_object();
    JSON_Object *json_obj = json_value_get_object(json_val);

    size_t size = 50;
    char *s = (char*)calloc(50, sizeof(char));
    
    printf("username=");
    fgets(s, 2, stdin);
    getline(&username, &size, stdin);
    char* aux = strchr(username, '\n');
    username[aux - username] = '\0';
    if(strchr(username, ' ')){
        printf("Error! No white spaces allowed! ヾ｜￣ー￣｜ﾉ\n");
        *ok = 1;
        return NULL;
    }
    json_object_set_string(json_obj, "username", username);

    printf("password=");
    getline(&password, &size, stdin);
    aux = strchr(password, '\n');
    password[aux - password] = '\0';
    if(strchr(password, ' ')){
        printf("Error! No white spaces allowed! ヾ｜￣ー￣｜ﾉ\n");
        *ok = 1;
        return NULL;
    }
    json_object_set_string(json_obj, "password", password);
    return json_serialize_to_string_pretty(json_val);
}

char* add_book_payload(int *ok){
    JSON_Value *json_val = json_value_init_object();
    JSON_Object *json_obj = json_value_get_object(json_val);
    
    printf("**Complete all fields!**\n");
    char *title = (char*)calloc(LINELEN, sizeof(char));
    char *author= (char*)calloc(LINELEN, sizeof(char));
    char *genre = (char*)calloc(LINELEN, sizeof(char));
    char *publisher = (char*)calloc(LINELEN, sizeof(char));
    char page_count[6];
    size_t size = LINELEN;
    char *s = (char*)calloc(LINELEN, sizeof(char));

    printf("title=");
    fgets(s, 2, stdin);
    getline(&title, &size, stdin);
    char* aux = strchr(title, '\n');
    title[aux - title] = '\0';
    json_object_set_string(json_obj, "title", title);
    
    printf("author=");
    getline(&author, &size, stdin);
    aux = strchr(author, '\n');
    author[aux - author] = '\0';
    json_object_set_string(json_obj, "author", author);

    printf("genre=");
    getline(&genre, &size, stdin);
    aux = strchr(genre, '\n');
    genre[aux - genre] = '\0';
    json_object_set_string(json_obj, "genre", genre);

    printf("publisher=");
    getline(&publisher, &size, stdin);
    aux = strchr(publisher, '\n');
    publisher[aux - publisher] = '\0';
    json_object_set_string(json_obj, "publisher", publisher);

    printf("page_count=");
    scanf("%s", page_count);
    json_object_set_number(json_obj, "page_count", atoi(page_count));
    
    for (int i = 0 ; i < strlen(page_count); i++) {
        if (page_count[i] < 48 || page_count[i] > 57) {
            printf("Wrong input! The page count must be a number!\n");
            printf("\n");
            *ok = 1;
            break;
        }
    }

    free(title);
    free(author);
    free(genre);
    free(publisher);
    return json_serialize_to_string_pretty(json_val);   
}

int main(int argc, char *argv[]) {
    int sockfd, status;
    char *mesaj_req, *rasp, *status_msg, *cookie = NULL, *tokenJWT = NULL;
    char comanda[BUFLEN], username[50], password[50], v[15], s[20];
   
    while (1) {
        sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
        scanf("%s", comanda);

// ****************************************************************************************************************REGISTER  
        if (strcmp(comanda, "register") == 0) { // Daca se doreste creearea unui cont nou de utilizator
            int ok = 0;
            char *payload = reg_log_payload(username, password, &ok); // citesc username-ul si parola
            if(ok == 1) { // daca unul dintre ele are spatii
                printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
                continue; // comanda este ignorata
            }
            // Creez request
            mesaj_req = compute_post_request("34.241.4.235", "/api/v1/tema/auth/register", "application/json", &payload, 1, NULL, 0, NULL, 0);
            // Trimit request la server
            send_to_server(sockfd, mesaj_req);
            // Primesc raspuns de la server
            rasp = receive_from_server(sockfd);
            sscanf(rasp,"%s%d%s", v, &status, s);
            
            if (status == 201) { // Daca user-ul a fost creat cu succes 
                // initializez cookie si token cu NULL
                cookie = NULL;
                tokenJWT = NULL; 
                printf("\nNew user account created! ヾ(●＾∇＾●)ﾉ\n Username: %s \n Password: %s\n", username, password);
            } else {
                // Daca user-ul nu a fost creat cu succes
                status_msg = basic_extract_json_response(rasp);
                printf("%s\nThe user account counldn't be created!\n", status_msg);    
            }

            json_free_serialized_string(payload);
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************LOGIN           
        } else if (strcmp(comanda, "login") == 0) { // Daca user-ul vrea sa se logheze
            int ok = 0;
            char *payload = reg_log_payload(username, password, &ok); // citesc username-ul si parola
            if(ok == 1) { // daca unul dintre ele are spatii
                printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
                continue; // comanda este ignorata
            }
            mesaj_req = compute_post_request("34.241.4.235", "/api/v1/tema/auth/login", "application/json", &payload, 1, NULL, 0, NULL, 0);
            send_to_server(sockfd, mesaj_req);
            rasp = receive_from_server(sockfd);
            sscanf(rasp, "%s%d%s", v, &status, s);
           
            if (status == 200) { //Daca comanda a fost realizata cu succes
                char* line = strstr(rasp , "connect.sid");
                cookie = strtok(line, ";"); //retin cookie-ul
                tokenJWT = NULL;
                printf("\nLogging in as %s ... SUCCESS (´• ω •`)ﾉ\n", username);
            } else { //Daca comanda nu a fost realizata cu succes
                status_msg = basic_extract_json_response(rasp);  
                printf("%s\nNo user found with the credentials used!\n", status_msg);
            }
            
            json_free_serialized_string(payload);
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************ENTER_LIBRARY  
        } else if (strcmp(comanda, "enter_library") == 0) { //Daca user-ul solicita acces la bilblioteca
            mesaj_req = compute_get_request("34.241.4.235", "/api/v1/tema/library/access", NULL, &cookie, 1, NULL, 0);
            send_to_server(sockfd, mesaj_req);
            rasp = receive_from_server(sockfd);
            sscanf(rasp, "%s%d%s", v, &status, s);

            if (status == 200) { //Daca comanda a fost realizata cu succes
                char* line = strstr(rasp, "token");
                tokenJWT = line + strlen("token") + 3; //retin token-ul JWT
                tokenJWT[strlen(tokenJWT) - 2] = '\0';
                printf("\nAccess to library granted!\n Welcome! ＼（＾○＾）人（＾○＾）／\n");
            } else { //Daca comanda nu a fost realizata cu succes
                status_msg = basic_extract_json_response(rasp);
                printf("%s\n", status_msg);
            }
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************GET_BOOKS              
        } else if (strcmp(comanda, "get_books") == 0) { //Daca user-ul crea sa vaad cartile din biblioteca 
            mesaj_req = compute_get_request("34.241.4.235", "/api/v1/tema/library/books", NULL, &cookie, 1, &tokenJWT, 1);
            send_to_server(sockfd, mesaj_req);
            rasp = receive_from_server(sockfd);
            sscanf(rasp, "%s%d%s", v, &status, s);
            status_msg = basic_extract_json_response(rasp);

            if (status == 200) {  //Daca comanda a fost realizata cu succes
                if (status_msg == NULL) // daca nu a fost gasita nicio carte
                    printf("Library is empty!\n Looks like some angry spiders made home here... /╲/╲╭(ఠఠ益ఠఠ)╮/╲╱╲ \n");
                else {
                    status_msg[strlen(status_msg) - 1] = '\0';
                    printf("\nBooks in the library ଘ(੭ˊᵕˋ)੭* ੈ✩‧₊˚:\n\n%s\n", status_msg); //afisez cartile
                }                
            } else { //Daca comanda nu a fost realizata cu succes
                printf("%s\n", status_msg);
                printf("No access to library!\n");
            }   
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************GET_BOOK              
        } else if (strcmp(comanda, "get_book") == 0) { //Daca user-ul vrea sa vada o anumita carte din biblioteca
            int id;
            printf("id=");
            scanf("%d", &id); // citesc ID-ul cartii cautate
            char url[35];
            sprintf(url, "/api/v1/tema/library/books/%d", id); // creez url-ul catre cartea cu ID-ul dat
            
            mesaj_req = compute_get_request("34.241.4.235", url, NULL, &cookie, 1, &tokenJWT, 1);
            send_to_server(sockfd, mesaj_req);
            rasp = receive_from_server(sockfd);
            sscanf(rasp, "%s%d%s", v, &status, s);
            status_msg = basic_extract_json_response(rasp);

            if (status == 200) { //Daca comanda a fost realizata cu succes
                if (status_msg == NULL) // daca nu a fost gasita cartea cu ID-ul cautat
                    printf("Library is empty!\n Looks like some angry spiders made home here... /╲/╲╭(ఠఠ益ఠఠ)╮/╲╱╲ \n");   
                else {
                    status_msg = status_msg + 2;
                    status_msg[strlen(status_msg) - 2] = '\0';
                    printf("\nBook found! My preciousssss (╯✧▽✧)╯:\n\n%s\n", status_msg); //afisez cartea si detaliile
                }
            } else { //Daca comanda nu a fost realizata cu succes
                printf("%s\n", status_msg);
                if (strstr(status_msg, "No book was found!")) // in functie de mesajul de eroare
                    printf("No book with ID %d found! ლ(ಠ_ಠ ლ)\n", id);
                else
                    printf("No access to library!\n");
            }  
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************ADD_BOOK  
        } else if (strcmp(comanda, "add_book") == 0) { //Daca user-ul vrea sa adauge o noua carte in biblioteca
            int ok = 0;
            char *payload = add_book_payload(&ok); // citesc detaliile despre carte
            if( ok == 1) { // daca detaliile cartii nu respecta cerintele
                printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
                continue; // ignor comanda
            }
            // Creez request
            mesaj_req = compute_post_request("34.241.4.235", "/api/v1/tema/library/books", "application/json", &payload, 1, &cookie, 1, &tokenJWT, 1);
            // Trimit request la server
            send_to_server(sockfd, mesaj_req);
            // Primesc raspuns de la server
            rasp = receive_from_server(sockfd);
            sscanf(rasp, "%s%d%s", v, &status, s);

            if (status == 200) {  //Daca comanda a fost realizata cu succes
                printf("\n New book was added! \( ˙▿˙ )/\( ˙▿˙ )/\n");
            } else { //Daca comanda nu a fost realizata cu succes
                status_msg = basic_extract_json_response(rasp);
                printf("%s\n", status_msg);
            }
            json_free_serialized_string(payload);
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************DELETE_BOOK  
        } else if (strcmp(comanda, "delete_book") == 0) { // Daca user-ul doreste sa stearga o anumita carte
            int id;
            printf("id=");
            scanf("%d", &id); // citesc ID-ul cartii cautate
            char url[35];
            sprintf(url, "/api/v1/tema/library/books/%d", id); // creez url-ul catre cartea cu ID-ul dat

            mesaj_req = compute_delete_request("34.241.4.235", url, NULL, &cookie, 1, &tokenJWT, 1);
            send_to_server(sockfd, mesaj_req);
            rasp = receive_from_server(sockfd);
            sscanf(rasp, "%s%d%s", v, &status, s);

            if (status == 200) { //Daca comanda a fost realizata cu succes   
                printf("\nBook with ID %d was deleted! \n WOOSH! (ノ ˘_˘)ノ　ζ|||ζ　ζ|||ζ　ζ|||ζ\n", id); 
            } else { //Daca comanda nu a fost realizata cu succes
                status_msg = basic_extract_json_response(rasp);
                printf("%s\n", status_msg);
            }
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************LOGstatus_msg  
        } else if (strcmp(comanda, "logout") == 0) { // Daca user-ul doreste sa se delogheze
            mesaj_req = compute_get_request("34.241.4.235", "/api/v1/tema/auth/logout", NULL, &cookie, 1, NULL, 0);
            send_to_server(sockfd, mesaj_req);
            rasp = receive_from_server(sockfd);
            sscanf(rasp, "%s%d%s", v, &status, s);

            if (status == 200) { //Daca comanda a fost realizata cu succes
                // sterg cookie si token
                cookie = NULL;
                tokenJWT = NULL;
                printf("\nLogging out... SUCCESS\n See ya later! (・_・)ノ\n");
            } else { //Daca comanda nu a fost realizata cu succes
                status_msg = basic_extract_json_response(rasp);
                printf("%s\nCouldn't log out!", status_msg);
            }
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
// ****************************************************************************************************************EXIT  
        } else if (strcmp(comanda, "exit") == 0) { // Daca user-ul doreste sa paraseasca bilbioteca
            printf("Exiting library...SUCCESS\n The bear guard suspects you stole some books...ʕ •̀ ω •́ ʔ Ignore him.\n");
            exit(0);
// ****************************************************************************************************************WRONG COMMAND  
        } else { // Daca comanda introdusa este gresita
            printf("Command not found!\n");
            printf("﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎﹎\n");
        }
    }

    free(cookie);
    free(status_msg);
    free(tokenJWT);
    free(mesaj_req);
    free(rasp);
    close_connection(sockfd);
    return 0;
}
