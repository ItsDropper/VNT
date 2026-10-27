# Alkuvaiheen sivutaulut

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Sivutus muuntaa virtuaaliosoitteita fyysisiksi muistiosoitteiksi ja voi rajoittaa muistialueiden käyttöä. VNTOS rakentaa käynnistyksessä alustavan identiteettikartoituksen, jossa virtuaaliosoite vastaa samaa fyysistä osoitetta. Se auttaa käynnistystä, mutta ei ole täydellinen virtuaalimuistijärjestelmä.

Tutki `os/arch/x86_64/boot.S`-tiedoston sivutauluja ja CR3-rekisterin asetusta. Virheellinen kartoitus voi aiheuttaa page fault -poikkeuksen ennen kuin konsoli ehtii tulostaa mitään.

**Pohdi:** Mitä osoitteen muuntaminen tarkoittaa? Miksi identiteettikartoitus on alkuvaiheessa hyödyllinen?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: x86-64 long mode](./09-long-mode.md) · [Seuraava oppitunti: ELF ja linkkeriskripti →](./11-elf-and-linker.md)
