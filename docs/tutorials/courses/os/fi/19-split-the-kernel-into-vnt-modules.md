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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Miten jatkaa kurssin jälkeen](18-next-steps.md) · [Seuraava oppitunti: Keskeytykset, poikkeukset ja IDT →](20-interrupts-and-the-idt.md)
