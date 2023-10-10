            Tema #2 Aplicatie client-server TCP si UDP pentru
                    gestionarea mesajelor 

    Tema contine un fisier server.c, unul subscriber.c si altul
helpers.h. Am creat o interactiune dintre server si mai multi clienti
TCP, care se conecteaza la server. 

server.c :

- Creez socketii TCP si UDP cu functia 'socket';
- Fac bind la cei doi socketi;
- Golesc multimile de descriptori si inserez socketii tcp si udp in multimea de descriptori de citire

   while :

   - verificam daca:
   1. Daca am primit un socket de la un client TCP(request);
   2. Daca serverul a scris ceva la tastatura;
   3. Daca am primit o comanda de la un client TCP;
   4. Daca am introdus un client UDP.

   1. Cand facem accept la socket, primim de la clientul TCP IP-ul si PORT-ul.
   Iar cand facem receive, primim in buffer ID ul clientului TCP. Apoi, afisam
   mesajul cerut si adaugam clientul intr-un vector de clienti(filedescriptori). 
   Fiecare filedescriptor din acest vector va primi la randul lui topicurile 
   si SF-ul atunci cand se va face un subscribe.
   2. In acest caz verificam daca serverul a scris comanda 'exit' si inchidem
   toti socketii.
   3. In acest caz, verificam daca un client(filedescriptor) a facut subscribe\unsubscribe
   Cazul subscribe: Inseram in vectorul de clienti topicul si SF-ul corespunzator filedescriptorului.
   si in vectorul online inseram filedescriptorul in dreptul unui topic.
   Cazul unsubscribe: Eliminam din vectorul de clienti topicul si SF-ul corespunzator
   filedescriptorului. Iar din vectorul online, eliminam filedescriptorul din dreptul
   unui topic.
        Daca in schimb nu se mai primesc biti de la clientul TCP, inseamna
   ca el este deconectat. In acest caz, trebuie sa afisam comanda de deconectare
   cu id-ul corespunzator clientului. Totodata, stergem din vectorul de clienti
   toate topicurile corespuzatoare acestuia, doar ca le inseram intr-un vector de
   topicuri deoarece vrem sa tinem evidenta in functie de id a clientilor care
   au SF-ul egal cu 1. Plus, am un vector care imi zice cati clienti s-au deconectat
   cu SF-ul egal cu 1.
        Dupa, stergem socketul si actualizam din nou valoarea maxima a filedescriptorului.
   4. Facem recvfrom de la socketul de UDP si construim pachetul cu informatii pe care il trimitem
   la fiecare socket(la clientii TCP) abonat la topicul primit de la clientul UDP. Eu avand in vectorul de
   clienti stocate topicurile, pentru fiecare filedescriptor in parte. 
   Datele ce vin de la clientul UDP le convertesc in functia de 'decodificare'
   in functie de valoarea lui type.


Dupa executia buclei whil, se face clean la toti socketii.

subscriber.c :

    - In prima faza, conectez socketul creat si trimit id-ul citit catre server.
    - Fac similar select si verific daca s-a citit ceva de la tastatura.
    - Daca clientul a dat exit, dau break , iar serverul va sti sa stearga filedescriptorul
    si sa afiseze mesajul de deconectare a clientului.
    - Daca clientul a dat comanda de subscribe, unsubscribe, creez o structura
    care contine informatiile corespuzatoare comenzii si trimit structura serverului.
    - Cand ies din while, dau close la socket.
    - Verificam daca avem filedescriptor venit de la server. Daca da, atunci facem receive
    la datele trimise de server catre clientul TCP si le afisam cum este specificat in enunt.
