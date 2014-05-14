## Gildia Złodziei - Sprawozdanie

### Autorzy
+ **106593** - Szczepański Adam
+ **106596** - Czajka Mateusz

### Algorytm

Dla każdego procesu złodzieja możemy wyodrębnić 3 następujące po sobie czynności:
+ dobór złodziei w pary
+ papierkowa robota (przydzielenie domu)
+ rabunek domu arystokracji

#### Stałe
```
P - liczba miejsc w urzędzie
Z - liczba złodziei
D - liczba domów
```

##### Dobór w pary

<code>Z</code> procesów może przebywać w sekcji krytycznej doboru parnetów. Procesy utrzymują kolejkę FIFO związaną z tą czynnością. W momencie trafienia na kolejkę proces z nieparzystą pozycją <code>i</code> na kolejce rozpoczyna nasłuchiwanie na wiadomość od procesu który trafi na nią z pozycją <code>i + 1</code>. Procesy które trafią na kolejkę z pozycją <code>i + 1</code> wysyłają wymienioną wyżej wiadomość.

##### Papierkowa robota
<code>2*floor(P/2)</code> procesów może dokonywać papierkowej roboty w urzędzie (sekcja krytyczna). Wykorzystana jest ta sama kolejka co przy doborze w pary - nie ma potrzeby wysyłania dodatkowych komunikatów. Po zakończeniu dokumentowania proces z nieparzystą pozycją w kolejce wysyła komunikat o opuszczeniu sekcji krytycznej i oba procesy są usuwane z kolejki.

##### Rabowanie domu
Po przydzieleniu domu proces o nieparzystej pozycji w porzedniej kolejce ubiega się o dostęp do konkretnego domu - każdy dom to odzielna sekcja krytyczna (<code>D</code> kolejek). W sekcji krytycznej może przebywać tylko jeden proces jako, że dany dom może być rabowany tylko przez jedną parę złodzieji. Po rabunku dany dom jest oznaczony przez pewien czas jako niedostępny.

### Algorytm rozwiązania

#### Założenia przyjętego modelu komunikacji
+ asynchroniczny system z wymianą komunikatów
+ topologia połączeń **każdy z każdym**
+ kanały komunikacyjne typu FIFO


#### Omówienie algorytmu

##### Główna pętla
```cpp
while (!koniec) {
  komunikacja();
  konkretny_stan();
  zwolnienie_zasobow();
}
```
##### Stany
1. Partnership_insert
  + broadcast do pozostałych procesów z tagiem <code>REQUEST_TAG</code> (chęć wejście na kolejkę partnerów).
  + rozpoczęcie nasłuchiwania na zgodę na wejście na ww. kolejkę <code>CONFIRM_TAG</code>
  + przejście do stanu <code>Partnership_wait_for_confirm</code>
2. Partnership_wait_for_confirm
  + sprawdzenie czy przyszła zgoda od wszytkich złodziei
  + przejście do stanu <code>Partnership_wait_for_top</code>
3. Partnership_wait_for_top
  + synchronizacja
  + przejście do stanu <code>Partnership_critical_section</code>
5. Partnership_critical_section
  + jeśli pozycja w kolejce jest nieparzysta
    + nasłuchuj na partnera, tag <code>PARTNER_TAG</code> 
    + przejdź do stanu <code>Partnership_wait_for_partner</code>
  + jeśli pozycja w kolejce jest parzysta
    + zapisz id złodzieja poprzedzającego w kolejce
    + przejdź do stanu <code>Partnership_notify_partner</code>
6. Partnership_wait_for_partner
  + sprawdzaj aż nie przyjdzie wiadomość od partnera
  + przejdź do stanu <code>Docs_wait_for_top</code>
7. Partnership_notify_partner
  + powiadom partnera: tag <code>PARTNER_TAG</code>
  + przejdź do stanu <code>Docs_start_waiting_for_partner</code>
8. Docs_wait_for_top
  + oczekuj na wejście do sekcji krytycznej
  + przejdź do stanu <code>Docs_critical_section</code>
9. Docs_critical_section
  + po upływie czasu potrzebnego na wypełnienie dokumentów przejdź do stanu <code>Partnership_release</code>
10. Partnership_release
  + wyślij do pozostałych złodziei informację o zwolnieniu pozycji w kolejce: tag <code>RELEASE_TAG</code>
  + poinformuj partnera, że papierkowa robota jest zakończona, tag <code>PARTNER_TAG</code>
  + przejdź do stanu <code>House_start_waiting_for_partner</code>
11. Docs_start_waiting_for_partner
  + rozpocznij oczekiwanie na partnera, tag <code>PARTNER_TAG</code>
  + przejdź do stanu <code>Docs_wait_for_partner</code>
12. Docs_wait_for_partner
  + w momencie otrzymania potwierdzenia od partnera (<code>PARTNER_TAG</code>) przejdź do stanu <code>House_request_entry</code>
13. House_request_entry
  + wyślij do wszystkich złodziei informację o chęci wejścia do danego domu, tag: <code>REQUEST_TAG</code>
  + rozpocznij oczekiwanie na potwierdzenie ww. wiadomości (<code>CONFIRM_TAG</code>)
  + przejdź do stanu <code>House_wait_for_confirm</code>
14. House_wait_for_confirm
  + w momencie otrzymania potwierdzenia wejdź do sekcji krytycznej danego domu - przejdź do stanu <code>House_critical_section</code>
15. House_critical_section
  + zrabuj dom (oczekuj aż minie określony czas)
  + dodaj dom na kolejkę zasobów do zwolnienia (z określonym czasem wygaśnięcia - <code>HOUSE_QUARANTINE_DURATION</code>)
  + przejdź do stanu <code>House_notify_partner</code>
16. House_notify_partner
  + poinformuj partnera że, kradzież została zakończona (<code>PARTNER_TAG</code>)
  + przejdź do stanu <code>Partnership_insert</code>
17. House_start_waiting_for_partner
  + rozpocznij nasłuchiwanie na wiadomość o zakończeniu kradzieży (<code>PARTNER_TAG</code>)
  + przejdź do stanu <code>House_wait_for_partner</code>
18. House_wait_for_partner
  + sprawdzaj czy kradzież została zakończona
  + przejdź do stanu <code>Partnership_insert</code>

#### Złożoność komunikacyjna
Komunikacja pojdeńczego procesu wysyła w czasie jednego cyklu:
  + proces który wejdzie na kolejkę partnerów jako **nieparzysty**
    + wysłanie **n-1** <code>REQUEST_TAG</code> - wejście na kolejkę partnerów
    + odbiór **n-1** <code>CONFIRM_TAG</code> - zgoda na wejście na kolejkę partnerów
    + odbiór **1** <code>PARTNER_TAG</code> - potwierdzenie przydzielenia patnera
    + wysłanie **n-1** <code>RELEASE_TAG</code> - zwolnienie miejsca na kolejce patnerów
    + wysłanie **1** <code>PARTNER_TAG</code> - informacja o zakończeniu wypełniania dokumentów
    + otrzymanie **1** <code>PARTNER_TAG</code> - informacja o zakończeniu kradzieży
  + proces który wejdzie na kolejkę partnerów jako **nieparzysty**
    + wysłanie **n-1** <code>REQUEST_TAG</code> - wejście na kolejkę partnerów
    + odbiór **n-1** <code>CONFIRM_TAG</code> - zgoda na wejście na kolejkę partnerów
    + wysłanie **1** <code>PARTNER_TAG</code> - potwierdzenie przydzielenia patnera
    + otrzymanie **1** <code>PARTNER_TAG</code> - informacja o zakończeniu wypełniania dokumentów
    + wysłanie **n-1** <code>REQUEST_TAG</code> - wejście na kolejkę danego domu
    + otrzymanie **n-1** <code>CONFIRM_TAG</code> - zgoda na wejście na kolejkę danego domu
    + wysłanie **1** <code>PARTNER_TAG</code> - informacja o zakończeniu kradzieży

#### Złożoność czasowa
Dla proces który wejdzie na kolejkę partnerów jako **nieparzysty** złożoność czasowa wynosi **6**, natomiast dla dla procesu który wejdzie jako **parzysty** złożoność czasowa wynosi **7**.

Łącznie dla procesu który wejdzie na kolejkę partnerów jako **nieparzysty** otrzymujemy <code>3 * (n-1) + 3 = 3n</code> komunikacji.

Dla procesu który wejdzie na kolejkę partnerów jako **parzysty** otrzymujemy <code>4 * (n-1) + 3 = 4n - 1</code> komunijacji.

#### Wzajemne wykluczanie
W celu realizacji pierwszej sekcji krytycznej wykorzystany jest algorytm Lamporta, dla drugiej jest to algorytm Ricarta-Agrawalli.

### Wnioski
Dzięki wprowadzonym modyfikacjom w stosunku do pierwotnej wersji (połączenie kolejek do doboru partnerów i dokumentacji) udało się znacznie ograniczyć liczbę komunikacji.
