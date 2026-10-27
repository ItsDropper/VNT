# Loppuprojekti: tee todennettava parannus

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Valitse yksi rajattu ongelma, ymmärrä nykyinen toteutus, muuta sitä ja todista testillä, että uusi käytös toimii. Hyvä projekti voi olla selkeämpi virheilmoitus, regressiotesti tai tarkka dokumentaatio todellisesta rajapinnasta.

## Sanasto

Hyväksymiskriteeri kertoo, mistä onnistuminen tunnistetaan. Regressiotesti estää tunnetun virheen palaamisen. Rajoitus kertoo, mitä toteutus ei vielä tue.

## Käytännön harjoitus

Kirjoita ennen koodaamista ongelma, tutkittavat tiedostot, nykyinen käytös, tavoite, testi ja palautussuunnitelma. Toteuta yksi asia kerrallaan ja tarkista diff sekä CI-tulos.

## Yleinen virhe ja turvallisuus

Älä valitse vaikeaa synkronointikorjausta vain siksi, että se kuulostaa hienolta. Se vaatii kääntäjärajapinnan, lukon ja rinnakkaisuustestien ymmärtämistä.

## Tarkista ymmärtäminen

Miten testi epäonnistuu, jos muutos ei toimi? Kirjoita hyväksymiskriteeri yhdellä lauseella.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä lopputulos ei ole suurin muutos vaan muutos, jonka tarkoitus ja toiminta ovat selvät. Kerro, mikä ongelma ratkaistiin, miten testi osoittaa sen ja mitkä rajoitukset jäivät.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Fyysinen muisti ja sivukehykset](26-physical-memory-and-page-frames.md) · [Seuraava oppitunti: ACPI, APIC ja usea CPU-ydin →](27-acpi-apic-and-smp.md)
