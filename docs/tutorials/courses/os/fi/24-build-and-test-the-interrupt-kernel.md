# Rakenna ja testaa keskeytyskernel

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Keskeytyksiä käyttävä kernel voi kääntyä mutta kaatua, kun CPU käyttää IDT:tä. Siksi build, käynnistys ja tapahtuman todentaminen ovat eri tarkistuksia. Testin pitää näyttää, että keskeytys saapui ja käsittelijä teki odotetun asian.

## Sanasto

Odotettu tulos kertoo, mistä onnistuminen tunnistetaan. Loki tallentaa tapahtumia. CI-ajo suorittaa automaattisia tarkistuksia palvelimella.

## Käytännön harjoitus

Aja projektin juuresta bash os/build.sh, käynnistä ISO QEMUssa ja seuraa sarjaporttia. Etsi konkreettinen tulos, kuten muuttunut laskuri. Tarkista GitHub Actionsin ajo ja lue epäonnistuneen vaiheen loki.

## Yleinen virhe ja turvallisuus

Kernelin käynnistyminen ei todista, että keskeytys toimitettiin. Tarkista erikseen ohjaimen alustus, IDT-merkintä, keskeytyksen salliminen ja kuittaus.

## Tarkista ymmärtäminen

Mikä havainto todistaisi, että käsittelijä todella suoritettiin?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Testaa yksi taso kerrallaan: build, käynnistys, keskeytyksen saapuminen ja sen vaikutus. Jos yksi taso epäonnistuu, korjaa se ennen seuraavan tuloksen tulkitsemista.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: PIT, PIC ja ajastimen tikit](23-pit-pic-and-timer-ticks.md) · [Seuraava oppitunti: Fyysinen muisti ja sivukehykset →](26-physical-memory-and-page-frames.md)
