# Rakenna ja testaa keskeytyksiä käyttävä kernel

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Keskeytyksiä käyttävä kernel pitää sekä rakentaa että käynnistää. Oikein kääntyvä koodi voi silti kaataa CPU:n, jos IDT, pino tai paluutila on väärä.

Aja projektin juuresta `bash os/build.sh`, käynnistä ISO QEMUssa ja tarkkaile sarjaportin viestejä. Suorita asiaankuuluvat shell-testit. Tarkista GitHub Actions -työnkulku ja lue epäonnistuneen vaiheen loki. Älä väitä testausta onnistuneeksi, jos sitä ei ole tehty.

**Pohdi:** Miten todentaisit, että keskeytys käsiteltiin oikeasti?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: PIT, PIC ja ajastimen tikit](./23-pit-pic-and-timer-ticks.md) · [Seuraava oppitunti: Fyysinen muisti ja sivukehykset →](./26-physical-memory-and-page-frames.md)
