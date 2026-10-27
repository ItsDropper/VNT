# VNT-kielen perusteet kernel-koodissa

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

VNTOSin tavoitteena on pitää kernelin tavallinen logiikka VNT-kielessä. Assemblya tarvitaan vain teknisillä rajoilla, kuten käynnistyksessä ja keskeytykseen siirryttäessä. Harjoittele muuttujia, funktioita, ehtoja, silmukoita, moduuleja ja osoittimia.

Kernelissä virheellinen osoitin voi kaataa koko järjestelmän. Freestanding-koodi toimii ilman tavallisen sovelluksen ajonaikaista ympäristöä, joten tiedostoja, dynaamista muistia tai standardikirjaston kaikkia ominaisuuksia ei voi olettaa olevan käytössä.

Avaa `os/kernel.vnt` ja seuraa alustuksen kulkua. Katso myös `os/kernel/shell.vnt`.

**Pohdi:** Mitä freestanding tarkoittaa? Miksi osoittimet vaativat erityistä huolellisuutta?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Turvallisen kehitysympäristön valmistelu](./03-setup.md) · [Seuraava oppitunti: Miten lähdekoodista syntyy käynnistettävä kernel →](./05-build-pipeline.md)
