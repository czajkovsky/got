## Gildia Złodziei - Sprawozdanie

### Autorzy
+ **106593** - Szczepański Adam
+ **106596** - Czajka Mateusz

### Algorytm

Dla każdego procesu złodzieja możemy wyodrębnić 3 następujące po sobie czynności:
+ dobór złodzieji w pary
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

#### Wzajemne wykluczanie
W celu realizacji pierwszej sekcji krytycznej wykorzystany jest algorytm Lamporta, dla drugiej jest to algorytm Ricarta-Agrawalli.
