# Mitä käynnistyksessä tapahtuu?

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Tietokoneen käynnistys ei siirry suoraan VNT-funktioon. Firmware käynnistyy ensin, sitten käynnistyslatain kuten GRUB lataa kernelin ja luovuttaa sille hallinnan. Kernelin alkuvaiheen koodi valmistelee CPU:n, pinon ja muistin.

Avaa `os/arch/x86_64/boot.S`. Etsi Multiboot2-otsake, pino, sivutaulujen valmistelu, siirtyminen 64-bittiseen tilaan ja kernelin aloituskohta. Älä muuta näitä kohtia ennen kuin ymmärrät niiden järjestyksen.

**Pohdi:** Kuvaile käynnistysketju firmwaresta VNT-koodiin. Miksi alkuvaiheessa tarvitaan assemblya?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Miten lähdekoodista syntyy käynnistettävä kernel](./05-build-pipeline.md) · [Seuraava oppitunti: GRUB ja Multiboot2 →](./07-grub-and-multiboot2.md)
