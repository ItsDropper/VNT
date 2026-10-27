# Testaa muutokset arvailun sijaan

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Käännös tarkistaa osan virheistä. QEMU-käynnistys kertoo, pääseekö kernel käyntiin. Shell-testit voivat tarkistaa yksittäisiä toimintoja, ja GitHub Actions näyttää automaattisten tarkistusten todellisen tuloksen. Yksi onnistunut taso ei korvaa muita.

Muutosprosessin perusrunko: kirjaa muutos, aja `bash os/build.sh`, käynnistä QEMU, suorita asiaankuuluvat testit ja tarkista CI-loki. Testin pitää tarkistaa ominaisuuden todellinen toiminta, ei vain tulostaa sanaa ”onnistui”.

**Pohdi:** Mitä käännöstesti kertoo ja mitä käynnistystesti kertoo?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Käynnistysongelmien selvittäminen](./15-debugging.md) · [Seuraava oppitunti: Ensimmäinen pieni kernel-muutos →](./17-first-kernel-change.md)
