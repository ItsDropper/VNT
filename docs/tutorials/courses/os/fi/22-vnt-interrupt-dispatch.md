# Käsittele keskeytyksiä VNT-kielellä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kun matalan tason sisääntulo on tallentanut tarvittavan tilan, VNT-koodi voi käsitellä tapahtuman merkityksen. Se voi tarkistaa vektorin, päivittää laskurin tai ohjata tapahtuman oikealle käsittelijälle.

## Sanasto

Dispatch tarkoittaa tapahtuman ohjaamista käsittelijälle. Tunnetulla vektorilla on määritelty toiminta. Tuntemattomalle tapahtumalle tarvitaan turvallinen oletuskäytös.

## Käytännön harjoitus

Avaa os/interrupts.vnt. Etsi kohta, jossa vektori tarkistetaan tai tapahtuma ohjataan. Seuraa yhden vektorin reittiä ja vertaa sitä os/arch/x86_64/interrupts.S-tiedostoon.

## Yleinen virhe ja turvallisuus

Keskeytyskontekstissa ei voi huolettomasti kutsua mitä tahansa funktiota. Lukkojen ja muistinhallinnan on noudatettava määriteltyä keskeytyskäytäntöä.

## Tarkista ymmärtäminen

Miksi sisääntulo ja tapahtuman käsittelypolitiikka erotetaan? Miten tuntemattomaan vektoriin pitäisi suhtautua?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Pidä assembly-rajapinta pienenä](21-minimal-assembly-entry-stubs.md) · [Seuraava oppitunti: PIT, PIC ja ajastimen tikit →](23-pit-pic-and-timer-ticks.md)
