# PIT, PIC ja ajastimen tikit

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

PIT on perinteinen ohjelmoitava ajastin ja PIC vanha keskeytysohjain. Ajastin voi tuottaa säännöllisiä tikkejä. Kernel voi laskea niiden avulla aikaa tai myöhemmin päättää, milloin tehtävä vaihdetaan. Tikki ei itsessään ole tehtäväajastin.

## Sanasto

IRQ on laitteistokeskeytyspyyntö. EOI kertoo keskeytysohjaimelle, että keskeytys on käsitelty. Tick on ajastintapahtuma. Scheduler valitsee, mikä tehtävä saa CPU:n.

## Käytännön harjoitus

Etsi ajastimen ja PICin alustus sekä tikin käsittelijä os/-hakemistosta. Seuraa tapahtumaketjua: IRQ, IDT, käsittelijä, tilan päivitys ja kuittaus. Tarkista lähdekoodista, mitä VNTOS oikeasti tekee.

## Yleinen virhe ja turvallisuus

Täysi scheduler tarvitsee tehtävärakenteet, rekisteritilan vaihdon, pinojen hallinnan ja selkeän keskeytyskäytännön. Tikin laskeminen ei vielä vaihda tehtäviä.

## Tarkista ymmärtäminen

Mitä tikki kertoo? Mitä scheduler tarvitsee ajastimen lisäksi?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Ajastintikki on tapahtuma, ei automaattisesti valmis ajanmittaus tai tehtävänvaihto. Selvitä, kuka päivittää laskurin, kuka kuittaa keskeytyksen ja mitä tapahtuu toistuvissa tapahtumissa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Käsittele keskeytyksiä VNT-kielellä](22-vnt-interrupt-dispatch.md) · [Seuraava oppitunti: Rakenna ja testaa keskeytyskernel →](24-build-and-test-the-interrupt-kernel.md)
