# hydroponika

Podlewanie cykliczne roślin hydroponiki
CPU wemos d1 mini
Wejścia
1) pomiar napięcia ADC do pinu A0 -> wymaga konfiguracji mnożnika w zależności od dzielnika napięć
2) pin kontrolny 'configBtn' - button -> po restarcie jeśli zwarty do masy to uruchamia portal umożliwający konfigurację, portal aktywny tak długo jak długo pin jest zwarty do masy, blokuje to też opcje deepsleep
3) pomiar temperatury ds18b20 (w planach)

Wyjscia
1) outPin D3, wyjscie sterujące pompki obiegowej układu
2) RST - zwarte z D0 wymaga tego deepSleep

Działanie
Układ po resecie sprawdza czy configBtn jest zwarty do masy, tak długo jak występuje stan niski nie układ nie przejdzie w tryb oszczędzania energii.
Po zmianie stanu na wysoki esp przejdzie w stan deepSleep jeśli minie czas pracy.

Jeśli jest tryb congifu (configBtn jest zwarty do masy) dostępny jest portal konfiguracyjny www

Na portalu jest możliwość ustalania czasu załączenie pompy, czasu deepSleep (po zakończeniu pracy pompy układ zasypia na określony czas)
Nastawy czasów zapisywane są w eeprom
Można odczytać stan akumulatora (jego napięcie)
Można odczytać temprerature (w planach)

Układ włączy pompkę na zadany czas, gdy ten czas minie przejdzie w stan deepSleep (gdy tryb konfigu=false) w przeciwnym wypadku tylko wyłączy pompke
Zmiana stanu pompki powoduje przekazanie logu do mqtt na:
topic: /hydro/<nazwa>/info 
treść wiadomości: {id:<nazwa>, t:<temp>, s:<on/off>, bat:<volt>, ip:<ip>, on:<onTime>, off:<offTime>, konf:<0/1>}
