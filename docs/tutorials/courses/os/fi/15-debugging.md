# Käynnistysongelmien selvittäminen

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Kernel voi pysähtyä ennen ensimmäistä tulostetta, jäädä odottamaan tai kaatua poikkeukseen. Selvitä ongelma järjestelmällisesti: toista se, lue ensimmäinen build-virhe, tarkista viimeinen sarjaportin viesti ja rajaa muutos yhteen kohtaan.

Jos tuloste loppuu ennen IDT:n alustusta, tutki ensin aiempia käynnistysvaiheita. Älä muuta useita alijärjestelmiä yhtä aikaa. Pidä muistiinpanot siitä, mikä muuttui ja mitä havaitsit.

**Pohdi:** Miksi ensimmäinen virhe on usein tärkeämpi kuin viimeinen? Miten rajaat ongelman tiettyyn vaiheeseen?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Sarjaportin komentotulkki](./14-the-serial-shell.md) · [Seuraava oppitunti: Testaa muutokset arvailun sijaan →](./16-testing.md)
