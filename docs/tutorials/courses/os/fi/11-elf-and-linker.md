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


## Syvempi harjoitus: seuraa koodia vaiheittain

Linkkeri ratkaisee symbolien sijainnit ja sijoittaa osiot. Jos symboli puuttuu, selvitä missä sen pitäisi olla määritelty ja onko tiedosto mukana buildissa. Älä vain poista virheilmoitusta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Alkuvaiheen sivutaulut](10-initial-paging.md) · [Seuraava oppitunti: Freestanding-ohjelma →](12-freestanding-vnt.md)
