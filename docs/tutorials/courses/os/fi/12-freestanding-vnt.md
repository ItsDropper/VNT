# Freestanding VNT -ohjelma

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Tavallinen sovellus käyttää käyttöjärjestelmän palveluita, kuten tiedostoja ja muistiallokointia. Freestanding-koodi ei voi olettaa näitä palveluita olevan. Kernelin pitää toteuttaa tai alustaa monet perustoiminnot itse.

Avaa `src/compiler/freestanding_x86.c` ja tutki, mitä toimintoja freestanding-kääntäjä käsittelee. Vertaa tätä `os/kernel.vnt`-tiedostoon ja sen alustuksen järjestykseen. Kielen syntaksi voi olla tuettu, vaikka vastaavaa ajonaikaista palvelua ei olisi käytettävissä.

**Pohdi:** Miksi kernel ei voi olettaa tavallisen sovelluksen ympäristöä?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ELF ja linkkeriskripti](./11-elf-and-linker.md) · [Seuraava oppitunti: Sarjaportin tulostus ja syöte →](./13-serial-io.md)
