# Tee komentotulkin ohjeesta ylläpidettävä käyttöliittymä

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Komentotulkin ohje on osa sen käyttöliittymää. Sen pitää kuvata oikeasti olemassa olevat komennot, ryhmitellä ne löydettäviksi ja pysyä automaattisten regressiotarkistusten piirissä. Tässä oppitunnissa parannetaan VNTOSin nykyistä komentotulkkia; uutta jäsennintä ei rakenneta eikä väitetä, että kaikki komennot hyväksyvät argumentteja.

## Toteutus

Avaa `os/kernel/shell.vnt` ja etsi `shell_run()`-funktion `help haara. Komentotulkki käyttää tällä hetkellä kiinteille komennoille erillisiä `str_eq`-tarkistuksia ja tekstille argumentteineen `starts_with(buffer, "echo ")`-tarkistusta. Tämä on varhaiselle kernelille tarkoituksella yksinkertainen rakenne: komentojen toiminta on näkyvää ja helppo jäljittää.

Ohje tulostaa komennot nyt kolmeen ryhmään:

- **Commands:** `help`, `about`, `clear`, `echo`, `history`, `uptime`.
- **Diagnostics:** `synctest`, `mem`, `memtest`, `vmtest`, `irq`, `mouse`, `smp`.
- **Power:** `reboot`, `halt`.

Viimeinen käyttöohjerivi kertoo, että `echo TEXT` tulostaa annetun tekstin ja `history` näyttää edellisen valmiin komennon. Tekstit tulostetaan suoraan konsoliin ilman käyttöliittymäkirjastoa tai dynaamista muistia.

## Miksi komentotaulukkoa ei vielä lisätä?

Komentokuvaajataulukko voisi myöhemmin sisältää komentojen nimet, kuvaukset ja käsittelijät. Käsittelijätaulukko edellyttää kuitenkin luotettavaa funktio-osoittimien ja epäsuorien kutsujen tukea freestanding-kääntäjässä sekä selvää argumenttien jäsennyspolitiikkaa. Älä lisää tällaista rakennetta vain lyhentääksesi `if`-lauseita. Varmista ensin, että kieli ja koodigeneraattori tukevat tarvittavia funktiotyyppejä oikein.

Toistaiseksi komentojen tarkistukset pysyvät eksplisiittisinä. Ohje ryhmittelee ne käyttäjälle, ja CI tarkistaa ryhmien näkyvän tulosteen. Näin ristiriita voidaan havaita väittämättä, että metadata-taulukko olisi jo komentojen ainoa totuuden lähde.

## Regressiotestaus

Työnkulku `.github/workflows/vntos-boot.yml` käynnistää kernelin QEMUssa, lähettää `help`-komennon emuloidun PS/2-näppäimistön kautta ja kerää sarjaportin tulosteen. Testi tarkistaa kolme ryhmäriviä ja käyttöohjerivin. Tämä auttaa havaitsemaan, jos näkyvä käyttöliittymä rikkoutuu vahingossa.

Tarkistus on rajattu: se todistaa, että odotettu teksti esiintyy tulosteessa. Se ei todista jokaisen listatun komennon toimivan, kaikkien komentojen olevan virheettömiä tai laitteiston toimivan oikealla tietokoneella. Muut tulostetarkistukset kattavat erikseen joitakin komentopolkuja; itsetestitulosteet tarkistavat osaa muisti-, synkronointi- ja virtuaalimuistitoiminnoista.

## Manuaalinen tarkistus

Rakenna ja käynnistä kernel vain projektin tuetussa QEMU-työnkulussa tai erillisessä QEMU-ympäristössä. Kirjoita komentokehotteeseen:

```text
help
```

Tulosteen odotettu rakenne:

```text
Commands: help, about, clear, echo, history, uptime
Diagnostics: synctest, mem, memtest, vmtest, irq, mouse, smp
Power: reboot, halt
Usage: echo TEXT prints TEXT; history shows the previous command.
```

Kokeile sitten komentoja `echo hello` ja `history`. Ensimmäisen pitäisi tulostaa `hello`; jälkimmäisen pitäisi näyttää edellinen valmis komento komentotulkin yhden komennon historian mukaisesti. Älä käytä `reboot`- tai `halt`-komentoa, ellet ole valmis pysäyttämään kyseistä QEMU-istuntoa.

## Harjoitukset

1. Lisää `uptime`-komennolle kuvaus, joka kertoo oikein sen käyttävän nimellisiä PIT-tikkejä kalibroidun seinäkellon sijaan.
2. Selitä, miksi `echo` käyttää etuliitetarkistusta ja useimmat muut komennot tarkkaa yhtäsuuruutta.
3. Lisää uusi komento ja päivitä samalla ohje sekä QEMU-tulosteen tarkistus.
4. Kuvaa, mitä kääntäjätukea ja testejä tarvitaan ennen funktio-osoitintaulukkoon siirtymistä.
5. Miksi ohjetekstin löytyminen CI-tulosteesta ei todista vastaavan komennon toimivan?

## Hyväksymiskriteerit

- [ ] Ohje listaa nykyiset komentotulkin komennot luettavissa ryhmissä.
- [ ] Käyttöohjerivi kuvaa `echo`- ja `history`-komennot oikein.
- [ ] QEMU CI tarkistaa kaikki ohjeryhmät ja käyttöohjerivin.
- [ ] Komentojen käsittely pysyy eksplisiittisenä; kääntäjälle ei oleteta tukematonta funktio-osoitinominaisuutta.
- [ ] Dokumentaatio erottaa tulosteen tarkistuksen täydellisestä toiminnallisesta tai laitteistotestauksesta.

---

[← Edellinen oppitunti](./35-terminal-decimal-uptime.md) · [Kurssin sisällysluettelo](./README.md)
