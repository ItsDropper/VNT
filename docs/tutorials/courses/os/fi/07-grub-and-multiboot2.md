# GRUB ja Multiboot2

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

GRUB on käynnistyslatain, joka lataa kernelin muistiin. Multiboot2 määrittelee tavan, jolla yhteensopiva kernel tunnistetaan ja käynnistystietoja välitetään. VNTOSin käynnistys-assembly sisältää tähän tarvittavan otsakkeen.

Avaa `os/arch/x86_64/boot.S` ja etsi otsakkeen kentät. Vertaa niitä build-skriptin ISO-paketointiin. Kernel voi kääntyä onnistuneesti, vaikka GRUB ei tunnista sitä tai ei pysty lataamaan sitä.

**Pohdi:** Mitä GRUB tekee? Miksi Multiboot2-otsakkeen rakenteella on väliä?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mitä käynnistyksessä tapahtuu?](./06-the-boot-sequence.md) · [Seuraava oppitunti: Käynnistys-assemblyn lukeminen →](./08-reading-boot-assembly.md)
