# Keskeytykset, poikkeukset ja IDT

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Keskeytys siirtää CPU:n käsittelijälle, kun laite tai muu tapahtuma vaatii huomiota. Poikkeus syntyy esimerkiksi CPU:n havaitsemasta virheellisestä muistiviittauksesta. IDT eli Interrupt Descriptor Table kertoo, mihin käsittelijään tietty keskeytysvektori ohjataan.

Avaa `os/arch/x86_64/interrupts.S` ja `os/interrupts.vnt`. Assembly hoitaa tarkan sisääntulon ja paluurajan; VNT-koodi käsittelee tapahtuman merkityksen. Pino ja rekisterit pitää käsitellä oikein.

**Pohdi:** Mikä ero on keskeytyksellä ja poikkeuksella? Mitä IDT tekee?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Jaa kernel VNT-moduuleihin](./19-split-the-kernel-into-vnt-modules.md) · [Seuraava oppitunti: Pidä assembly-rajapinta pienenä →](./21-minimal-assembly-entry-stubs.md)
