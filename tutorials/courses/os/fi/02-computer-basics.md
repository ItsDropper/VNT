# Tietokoneen osat kernelin näkökulmasta

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

CPU suorittaa käskyjä. RAM säilyttää aktiiviset tiedot nopeasti saatavilla, kun taas SSD tai HDD säilyttää tiedot virran katketessa. Kernel käyttää osoitteita muistiin ja ohjausrajapintoja laitteisiin. Keskeytys ilmoittaa tapahtumasta, joka vaatii CPU:n huomiota.

## Sanasto

Rekisteri on CPU:n pieni tallennuspaikka. Osoite kertoo, mistä muistista luetaan tai minne kirjoitetaan. Keskeytys siirtää suorituksen käsittelijään. Polling tarkoittaa, että CPU tarkistaa laitteen tilan itse.

## Käytännön harjoitus

Piirrä näppäimistön tapahtumaketju: näppäin, laiteohjain, kernel, komentotulkki. Merkitse, missä kohtaa laite voi ilmoittaa keskeytyksellä ja missä kohtaa kernel voi tarkistaa tilaa kyselyllä.

## Yleinen virhe ja turvallisuus

Fyysinen ja virtuaalinen osoite eivät aina ole sama asia. Ne voivat vastata toisiaan alkuvaiheen identiteettikartoituksessa, mutta sivutus voi muuntaa osoitteen.

## Tarkista ymmärtäminen

Mikä ero on RAM-muistilla ja tallennustilalla? Mikä ero on keskeytyksellä ja pollingilla?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

CPU ei ymmärrä tiedoston tai ikkunan käsitettä itsestään. Se suorittaa käskyjä ja lukee tai kirjoittaa muistia. Korkeamman tason ominaisuudet rakentuvat näiden mekanismien päälle.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mitä käyttöjärjestelmä tekee?](01-what-is-an-os.md) · [Seuraava oppitunti: Valmistele kehitysympäristö →](03-setup.md)
