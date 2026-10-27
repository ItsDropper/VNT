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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Käsittele keskeytyksiä VNT-kielellä](22-vnt-interrupt-dispatch.md) · [Seuraava oppitunti: Rakenna ja testaa keskeytyskernel →](24-build-and-test-the-interrupt-kernel.md)
