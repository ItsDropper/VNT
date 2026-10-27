# Keskeytysten käsittely VNT-kielellä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Kun assembly on siirtänyt hallinnan kernelille, VNT-koodi voi päätellä vektorin perusteella, mitä tapahtui. Tunnetut vektorit pitää käsitellä oikein ja tuntemattomiin tapahtumiin suhtautua turvallisesti.

Avaa `os/interrupts.vnt` ja seuraa käsittelyä. Vertaa sitä `os/arch/x86_64/interrupts.S`-tiedostoon. Keskeytyskäsittelijän kannattaa olla lyhyt; hitaiden toimintojen tai lukkojen käyttö vaatii erityistä huolellisuutta.

**Pohdi:** Miksi matalan tason sisääntulo ja tapahtuman käsittelypolitiikka erotetaan?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Pidä assembly-rajapinta pienenä](./21-minimal-assembly-entry-stubs.md) · [Seuraava oppitunti: PIT, PIC ja ajastimen tikit →](./23-pit-pic-and-timer-ticks.md)
