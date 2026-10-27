# Komentohistorian haku ja käyttöajan diagnostiikka

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Paranna tekstipohjaista VNTOS-komentotulkkia lisäämättä graafista työpöytää tai kekomuistia. Tässä oppitunnissa lisätään edellisen komennon haku PS/2-nuoli ylös -näppäimellä, `history`-komento, `uptime`-diagnostiikka ja QEMU-tarkistus oikealle näppäimistöpolulle.

Toteutus on tiedostoissa `os/ps2.vnt`, `os/kernel/shell.vnt` ja `os/arch/x86_64/boot.S`. Syöte luetaan edelleen pollauksella. Tuloste menee VGA-tekstinäyttöön ja COM1-sarjaporttiin.

## Seuraa komentotulkin polkua

1. `keyboard_poll()` lukee 8042-ohjaimen tila- ja dataportteja.
2. `keyboard_translate()` muuntaa tavalliset Set 1 -skannauskoodit merkeiksi.
3. Laajennettu PS/2-näppäin alkaa `0xE0`-etuliitteellä. Ajuri muistaa etuliitteen seuraavaan koodiin asti.
4. Nuoli ylös palautetaan sisäisenä arvona `257`, ei tulostettavana ASCII-merkkinä.
5. `shell_run()` tunnistaa arvon 257 ja kopioi edellisen komennon aktiiviseen syötepuskuriin.
6. `console_putc()` tulostaa haetun tekstin VGA-näyttöön ja sarjaporttiin.

Sisäinen näppäinarvo on tavumerkkien alueen ulkopuolella, joten sitä ei voi sekoittaa tulostettavaan merkkiin. Arvo `256` tarkoittaa, ettei merkkiä ole saatavilla.

## E0-etuliitteen käsittely

Laajennettu näppäin saapuu useampana skannauskooditavuna. Jos jokainen `0xE0` palautetaan heti, seuraavan tavun merkitys katoaa. Ajuri käyttää tilapaikkaa 177:

```vnt
if state_get(177) == 1 {
    state_set(177, 0)
    if scan == 0x48 {
        return 257
    }
    return 256
}
if scan == 0xe0 {
    state_set(177, 1)
    return 256
}
```

Nuolen ylös painalluskoodi on `0x48`. Vapautuskoodit ohitetaan, joten näppäimen vapautus ei lisää merkkiä. Tuntemattomat laajennetut näppäimet ohitetaan, jotta ne eivät riko komentoriviä.

## Kiinteä historia ilman kekomuistia

VNTOSissa ei vielä ole yleiskäyttöistä kekomuistia eikä erillistä terminaalirivioliota. Edellinen komento tallennetaan tiedostossa `os/arch/x86_64/boot.S` määriteltyyn jaettuun tilamuistiin.

| Tilapaikat | Tarkoitus |
| --- | --- |
| 176 | Fyysisen varaajan next-fit-hakukohta; älä käytä uudelleen |
| 177 | Laajennetun näppäimen etuliite odottaa |
| 178 | Edellisen komennon pituus |
| 179 | Varattu |
| 180–307 | Edellisen komennon merkit, enintään 127 merkkiä ja lopetusmerkki |

Komentotulkki tallentaa valmiin, ei-tyhjän komennon vasta käsittelyn jälkeen. Näin `history` näyttää sitä edeltäneen komennon eikä itseään. Tyhjä Enter ei korvaa historiaa. Syöteraja ja historian kapasiteetti ovat molemmat 127 merkkiä.

Nuoli ylös tyhjentää nykyisen rivin näytöltä, kopioi tallennetut merkit syötepuskuriin ja tulostaa ne. Enter suorittaa haetun komennon. Tämä on yhden komennon historia, ei rengaspuskuri.

## Komennot ja ajastimen merkitys

- `history` tulostaa viimeisimmän valmiin, ei-tyhjän komennon tai kertoo historian olevan tyhjä.
- `uptime` tulostaa PIT-ajastimen tikit ja arvioidun sekuntimäärän.
- Nuoli ylös palauttaa edellisen komennon muokattavalle riville.

PIT on asetettu noin sataan keskeytykseen sekunnissa, joten `ticks / 100` on vain arvio, ei seinäkello. Emulaattorin ajoitus, väliin jääneet tikit ja ajastimen tulevat muutokset voivat vaikuttaa tulokseen. Raakatikkien tulostaminen näyttää, mihin arvio perustuu.

## Varmista oikea näppäimistöpolku

Työnkulku `.github/workflows/vntos-boot.yml` käynnistää QEMUn neljällä virtuaalisella CPU:lla ja syöttää näppäimet QEMUn näppäimistömonitorin kautta. Se kirjoittaa `echo test`, painaa nuolta ylös ja Enteriä, minkä jälkeen suorittaa `history`- ja `uptime`-komennot. Työnkulku tarkistaa haetun komennon ja uuden tulosteen.

Käsiajo projektin juuressa:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -smp 4 -serial stdio
```

Odotettu vuorovaikutus:

```text
VNTOS> echo test
test
VNTOS> [paina nuolta ylös]
VNTOS> echo test
test
VNTOS> history
Last command: echo test
VNTOS> uptime
Timer ticks: 0x...
Approximate uptime seconds: 0x...
```

Tarkat tikkiarvot riippuvat ajoituksesta. Tarkista commitin GitHub Actions -tulos: lähdekoodimuutos tai uskottava tuloste ei todista QEMU-testin valmistuneen.

## Virhetilanteet ja rajoitukset

1. **Ei historiaa:** nuoli ylös ei muuta riviä ja `history` ilmoittaa tyhjästä historiasta.
2. **Tyhjä komento:** ei korvaa edellistä komentoa.
3. **Pitkä komento:** syöte rajataan 127 merkkiin.
4. **Näppäimen vapautus:** ohitetaan eikä palauteta merkkinä.
5. **Tuntematon laajennettu näppäin:** ohitetaan.
6. **Sarjaportin navigointi:** tavallinen sarjasyöte toimii, mutta ANSI-escape-jonoja ei tulkita navigointinäppäimiksi.
7. **Ajastimen tarkkuus:** sekuntiarvio on likimääräinen eikä sovi määräaikoihin, kalenteriaikaan tai turvallisuuspäätöksiin.

Tämä ei ole täysi terminaaliemulaattori. Usean komennon historia, nuoli alas, kohdistimen vapaa liike, rivin keskelle lisääminen/poistaminen, sarjaportin ANSI-parseri ja asynkroninen näppäimistökeskeytyskäsittelijä puuttuvat. IRQ1 pysyy peitettynä, koska näppäimistöä luetaan pollauksella.

Seuraavaksi kannattaa erottaa rajattu rivieditori komentojen käsittelystä ja lisätä kohdistimen liike sekä poistaminen määritellyllä VGA-/sarjaporttikäytöksellä. Näppäimistökeskeytyksiä ei pidä ottaa käyttöön ennen kuin käsittelijä ja syötepuskuriomistajuus ovat turvallisia.

## Harjoitukset

1. Miksi `0xE0` ei yksin ole valmis navigointinäppäin?
2. Miksi nuoli ylös palautetaan arvona 257 eikä ASCII-tavuna?
3. Miksi tilapaikka 176 on jätettävä varaajan käyttöön?
4. Miksi historia tallennetaan vasta komennon käsittelyn jälkeen?
5. Suunnittele kahden komennon historia varaajan ja SMP-tilan päällekkäisyyttä välttäen; dokumentoi jokainen tilapaikka.
6. Miksi PIT-tikkien jakaminen sadalla on vain arvio?
7. Lisää CI-tarkistus vasta, kun QEMU-vieraan tarkka tuloste on tiedossa.

## Hyväksymiskriteerit

- [ ] Tavallisten Set 1 -merkkien muunnos säilyy.
- [ ] E0-etuliite ja nuoli ylös tunnistetaan.
- [ ] Nuoli ylös palauttaa viimeisimmän ei-tyhjän komennon.
- [ ] `history` näyttää edeltävän komennon.
- [ ] `uptime` näyttää raakatikit ja merkitsee sekunnit arvioksi.
- [ ] Tilapaikat eivät mene päällekkäin varaajan tai SMP-kenttien kanssa.
- [ ] CI tarkistaa oikean PS/2-polun ja uuden tulosteen.
- [ ] Suomen- ja englanninkielinen oppitunti kuvaavat saman toiminnan ja rajoitukset.

---

[← Edellinen oppitunti](./32-reserved-memory-map-precedence.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./34-terminal-editable-line-editor.md)
