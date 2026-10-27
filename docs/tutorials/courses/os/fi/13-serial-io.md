# Sarjaportin tulostus ja syöte

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Sarjaportti tarjoaa kernelille yksinkertaisen tavan tulostaa viestejä. QEMU voi välittää sarjaportin tekstin terminaaliin, vaikka näyttöajuri ei olisi valmis. Siksi sarjaportti on hyödyllinen käynnistyksen ja virheiden tutkimisessa.

Etsi `os/`-hakemistosta sarjaportin alustuksen ja kirjoituksen koodi. Seuraa, missä käynnistysviestit tulostetaan ja mistä syöte luetaan. Vertaa QEMU-tulostetta siihen, mitä koodi tekee.

**Pohdi:** Miksi sarjaportti auttaa debuggaamisessa? Miksi tuloste ei todista kaikkien muiden osien toimivan?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Freestanding VNT -ohjelma](./12-freestanding-vnt.md) · [Seuraava oppitunti: Sarjaportin komentotulkki →](./14-the-serial-shell.md)
