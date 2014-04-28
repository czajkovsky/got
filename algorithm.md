### Algorytm
#### Stałe
```
P - liczba miejsc w urzędzie
Z - liczba złodziei
D - liczba domów
```
#### Kolejka procesów oczekujących na wejście do sekcji krytycznej
```
class WaitingQueue {
  insert(timestamp, pid);
  is_on_top(timestamp, pid);
  is_in_top(n, timestamp, pid);
  pop();
  pop(timestamp, pid);
};
```
#### Wątki

##### Wątek główny

```
while(true) {
  
  send(REQUEST);
  for each Z {
    recv(CONFIRM);
  }
  
  while(not_on_top) { // priorytetowa kolejka
    sleep();
  }
  
  // sekcja krytyczna doboru partnera
  if(nobody_is_waiting()) {
    send(RELEASE);
    // wyjście z sekcji krytycznej
    recv(READY);
  }
  else {
    send(RELEASE);
    // wyjście z sekcji krytycznej
    send(READY);
  }
  
  
  if(my_id < my_partner_id) {
    send(REQUEST);
    for each Z {
      recv(CONFIRM);
    }
  
    while(not_my_id < P / 2) {
      sleep(SMALL);
    }
    // sekecja krytyczna papierkowej roboty
    sleep(LARGE);
    assign_house();
    send(RELEASE);
    // opuszczenie sekcji papierkowej roboty
    send(HOUSE_ASSIGNED);
  }
  else {
    recv(HOUSE_ASSIGNED);
  }
  
  if(my_id < my_partner_id) {
    send(REQUEST);
    for each Z {
      recv(CONFIRM);
    }
    while(not_on_top) {
      sleep(SMALL);
    }
    // wejście do sekcji krytycznej danego domu
    sleep(LARGE);
    
    release_house_thread();
    
  }
}
```

##### Wątek związany z sekcjami krytycznymi
```
while(true) {
  recv(REQUEST); // id, timestamp, PQ_id
  send(CONFIRM); // timestamp jako argument
  PQ_id.add((id, timestamp));
}
```

##### Wątek związany ze zwolnieniem zasobów
```
while(true) {
  recv(RELEASE); // PQ_id, id
  if(PQ_id == kolejka_do_parowania) {
    if(no_one_waiting == -1) no_one_waiting = id;
    else no_one_waiting = -1;
  }
  PQ_id.remove(id);
}
```

##### Wątek zwalniający dom
```
sleep(LONG);
send(RELEASE); // house_PQ
```
