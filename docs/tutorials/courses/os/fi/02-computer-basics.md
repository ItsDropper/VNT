# Tietokone kernelin näkökulmasta

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

CPU suorittaa konekäskyjä. RAM säilyttää käytössä olevat tiedot, kun taas tallennuslaite säilyttää ne virran katketessa. Muistiosoite kertoo, mistä tietoa luetaan tai minne sitä kirjoitetaan. Keskeytys ohjaa CPU:n käsittelijään, kun jokin tapahtuma vaatii huomiota.

Kernel toimii lähellä laitteistoa. Sen täytyy joskus käsitellä rekistereitä, sivutauluja ja keskeytysohjaimia suoraan. Kääntäjä muuttaa VNT-koodin koneen ymmärtämäksi koodiksi, ja käynnistysketju valmistelee CPU:n ennen kernelin tavallista toimintaa.

**Pohdi:** Mikä ero on RAM-muistilla ja tallennuslaitteella? Milloin keskeytys on hyödyllinen?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mitä käyttöjärjestelmä tekee?](./01-what-is-an-os.md) · [Seuraava oppitunti: Turvallisen kehitysympäristön valmistelu →](./03-setup.md)
