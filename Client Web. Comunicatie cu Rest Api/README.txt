                                            TEMA3 - Client Web. Comunicatie cu Rest Api

        Am inceput prin a-mi defini toate api-urile corespunzatoare cererilor pe care le voi face. Totodata, in ceea ce priveste construirea
    payload-ului, am ales biblioteca cJSON (incluzand fisierele cJSON.c si cJSON.h in tema). Prin obtinerea ip-ului, vom putea deschide 
    conexiunea la server. Apoi verific comenzile citite de la tastatura. 
        Daca spre exemplu am citit un "register", urmate de un "username" si o "parola", in cazul acesta vom trimite un POST catre server 
    care va contine api-ul corespunzator REGISTERULUI, Host-ul, Content-Type-ul, Content-Length-ul ce reprezinta lungimea payload-ului. 
    Acestea sunt urmate in final de payload-ul construit cu ajutorul functiilor ajutatoare din cJSON.c. Chiar daca se putea face si 
    manual(sprintf de exemplu) aceasta construie a payload-ului, am ales totusi varianta asta deoarece este mai eficienta, estetica si 
    nu strica cunoasterea unei noi biblioteci ca aceasta. 
        Login-ul se face aproape identic cu register, singura diferenta importanta fiind faptul ca tratez cazul de preluare a cookie-ului de la 
    ultimul login reusit.
        La "enter_library", eu demonstrez ca sunt logat prin faptul ca trimit cookie-ul serverului in cererea de get. Evident ca daca nu as avea
    cookie-ul, serverul imi va intoarce eroare dupa ce trimit cererea. Idem ca si la cookie, iau in considerare ultimul token primit.
        La comanda "get_books" , in construirea cererii adaug si token-ul obtinut prin care demonstrez serverului ca am dreptul de acces la carti,
    utilizatorul fiind "intrat in librarie". Altfel, serverul intoarce eroare.
        La comanda "get_book", tot asa demonstrez ca am acces la biblioteca. Daca fie id-ul este invalid, fie n-am acces la biblioteca, serverul
    imi va intoarce eroare corespunzatoare.
        La comanda "add_book", folosec functiile din cJSON.c in construirea payload-ului atasat cererii. Demonstrez ca am acces la biblioteca, si
    atasez cererii inclusiv Content-Type si Content-Length.
        La comanda "delete_book", tot asa demonstrez ca am acces la biblioteca. Daca fie id-ul este invalid, fie n-am acces la biblioteca, serverul
    imi va intoarce eroare corespunzatoare.
        La comanda "logout", demonstrez ca sunt autentificat si trimit get-ul serverului.
        La comanda "exit", inchid socketul de server si dau logout.     


        

                    