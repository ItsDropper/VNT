# ELF ja linkkeriskripti

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

ELF on tiedostomuoto, joka kuvaa käännetyn ohjelman rakenteen. Linkkeri yhdistää objektitiedostot ja sijoittaa niiden osiot. Linkkeriskripti kertoo, mihin muistiosoitteisiin kernelin osat sijoitetaan ja mistä suoritus alkaa.

## Sanasto

.text sisältää yleensä koodia, .data alustettuja muuttujia ja .bss nollattavaa tai alustamatonta dataa. Symboli on funktion tai muuttujan tunniste. Linkkeriskripti määrittää muistiasettelua.

## Käytännön harjoitus

Etsi os/-hakemistosta linkkeriskripti ja avaa se. Vertaa sitä os/build.sh-tiedostoon. Selvitä, miten skripti välitetään linkkerille ja mikä kernel-tiedosto syntyy. Jos työkalut ovat käytössä, tutki ELF-tiedoston osioita readelfillä.

## Yleinen virhe ja turvallisuus

Puuttuva symboli on usein merkki siitä, että tiedosto ei ole mukana buildissa tai funktion nimi/rajapinta ei täsmää. Älä vain poista linkkerin virheilmoitusta.

## Tarkista ymmärtäminen

Mikä ero on objektitiedostolla ja ELF-kernelillä? Mitä linkkeriskripti määrittää?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Alkuvaiheen sivutaulut](10-initial-paging.md) · [Seuraava oppitunti: Freestanding-ohjelma →](12-freestanding-vnt.md)
