# Pidä assembly-rajapinta pienenä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Assemblya tarvitaan, kun CPU:n protokolla vaatii tarkkaa rekisterien, pinon tai ohjausrekisterien käsittelyä. Tavallinen päätöksenteko kannattaa pitää VNT-kielessä, jotta sitä on helpompi lukea ja testata.

Lue `os/arch/x86_64/interrupts.S` ja selvitä, mitä tietoja se välittää VNT-käsittelijälle. Älä siirrä kokonaisia ajureita tai muistinhallinnan logiikkaa assemblyyn vain helppouden vuoksi.

**Pohdi:** Nimeä yksi tehtävä, joka kuuluu assembly-rajapintaan, ja yksi tehtävä, joka kuuluu VNT-koodiin.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Keskeytykset, poikkeukset ja IDT](./20-interrupts-and-the-idt.md) · [Seuraava oppitunti: Keskeytysten käsittely VNT-kielellä →](./22-vnt-interrupt-dispatch.md)
