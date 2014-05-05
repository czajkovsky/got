## Gildia Złodziei - Sprawozdanie

### Autorzy
+ **106593** - Szczepański Adam
+ **106596** - Czajka Mateusz

### Algorytm

Dla każdego procesu złodzieja możemy wyodrębnić 3 następujące po sobie czynności:
+ dobór złodzieji w pary
+ papierkowa robota (przydzielenie domu)
+ rabunek domu arystokracji

##### Stałe
```
P - liczba miejsc w urzędzie
Z - liczba złodziei
D - liczba domów
```

#### Dobór w pary

<code>Z</code> proceów może przebywać w sekcji krytycznej doboru parnetów. Procesy utrzymują kolejkę FIFO związaną z tą czynnością. W momencie trafienia na kolejkę proces z parzystą pozycją (<code>i</code>) na kolejce rozpoczyna nasłuchiwanie na wiadomość od procesu który trafi na nią z pozycją <code>i + 1</code>. Procesy które trafią na kolejkę z pozycją nieparzystą wysyłają wymienioną wyżej wiadomość. Po doborze w parę proces o mniejszym ID usuwa oba procesy z kolejki.

#### Papierkowa robota
<code>floor(P/2)</code> procesów może dokonywać papierkowej roboty w urzędzie (sekcja krytyczna). <code>floor(P/2)</code> ponieważ tylko proces o mniejszym ID ubiega się o dostęp - proces z większym ID czeka na wiadomość od swojego partnera o zakończeniu procesu papierkowej roboty.

#### Rabowanie domu
Po przydzieleniu domu proces o mniejszym ID ubiega się o dostęp do konkretnego domu - każdy dom to odzielna kolejka FIFO (<code>D</code> kolejek). W sekcji krytycznej może przebywać tylko jeden proces jako, że dany dom może być rabowany tylko przez jedną parę złodzieji. Po rabunku dany dom jest oznaczony przez pewien czas jako niedostępny.
