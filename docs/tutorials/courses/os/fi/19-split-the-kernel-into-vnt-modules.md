# Jaa kernel VNT-moduuleihin

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Moduulit jakavat vastuut: käynnistys, keskeytykset, shell ja muistinhallinta ovat eri tehtäviä. Hyvä moduuli tarjoaa pienen rajapinnan ja pitää sisäiset yksityiskohdat erillään. Tavoite ei ole maksimoida tiedostojen määrää vaan tehdä vastuut ymmärrettäviksi.

## Sanasto

Moduuli on yhteen tehtävään keskittyvä koodiosa. Riippuvuus tarkoittaa, että osa tarvitsee toisen palvelua. Omistajuus kertoo, mikä osa saa muuttaa tiettyä dataa.

## Käytännön harjoitus

Tutki os/kernel.vnt, os/interrupts.vnt, os/mm/phys.vnt ja os/kernel/shell.vnt. Piirrä nuolet funktiokutsujen mukaan. Selvitä build-skriptistä, miten VNT-tiedostot otetaan mukaan käännökseen.

## Yleinen virhe ja turvallisuus

Tiedoston jakaminen ei yksin luo hyvää moduulirajaa. Jos kaikki osat muuttavat samaa globaalia tilaa ilman sääntöjä, kokonaisuus pysyy vaikeana ja virhealttiina.

## Tarkista ymmärtäminen

Piirrä kolmen moduulin riippuvuuskaavio. Mitä moduulijako ratkaisee ja mitä se ei ratkaise?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä moduuliraja vastaa kysymykseen: kuka on vastuussa tästä tiedosta ja toiminnosta? Jos moduulit tuntevat toistensa sisäiset yksityiskohdat, rajapinta voi olla epäselvä. Tiedostojen määrä ei itsessään mittaa laatua.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Miten jatkaa kurssin jälkeen](18-next-steps.md) · [Seuraava oppitunti: Keskeytykset, poikkeukset ja IDT →](20-interrupts-and-the-idt.md)
