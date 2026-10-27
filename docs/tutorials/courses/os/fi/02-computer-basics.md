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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mitä käyttöjärjestelmä tekee?](01-what-is-an-os.md) · [Seuraava oppitunti: Valmistele kehitysympäristö →](03-setup.md)
