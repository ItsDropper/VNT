# Testaa desimaalimerkin poiminta ilman konsolitulostusta

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Muotoilijaan on helpompi luottaa, kun sen aritmetiikka voidaan testata erillään laitteistotulostuksesta. VNTOS poimii nyt yhden kymmenkantaisen numeromerkin puhtaalla apufunktiolla `decimal_digit_at(value, divisor)`, joka sijaitsee tiedostossa `os/console.vnt`. Funktio ei kirjoita VGA:lle tai sarjaporttiin, varaa muistia eikä muuta kernelin tilaa.

## Apufunktion sopimus

Funktio hyväksyy ei-negatiivisen kokonaisluvun ja positiivisen jakajan. Tarkoitettu kutsuja antaa kymmenen potenssin: 1, 10, 100, 1000 ja niin edelleen. Paluuarvo lasketaan näin:

```text
(value / divisor) % 10
```

Kokonaislukujako siirtää valitun desimaalipaikan ykkösten paikalle; jakojäännös kymmenellä poimii kyseisen numeron. Esimerkiksi `decimal_digit_at(907, 100)` palauttaa 9 ja `decimal_digit_at(907, 10)` palauttaa 0. Funktio palauttaa arvon -1, jos luku on negatiivinen tai jakaja ei ole positiivinen.

Funktio ei tarkista, onko positiivinen jakaja todella kymmenen potenssi. Tämä on kutsujan vastuulla oleva ehto, joka on dokumentoitu, jotta kernelin apufunktio pysyy pienenä. Esimerkiksi jakaja 3 on sopimuksen ulkopuolella, vaikka itse aritmeettinen lauseke on määritelty.

## Muotoilijan käyttö

Nykyinen `console_decimal(value)` vastaa edelleen tulostuksesta: se käsittelee nollan, tulostaa miinusmerkin negatiivisille arvoille, valitsee jakajan, tulostaa numerot ja päättää rivin CRLF-merkkeihin. Jokaisen numeron kohdalla se kutsuu nyt `decimal_digit_at`-funktiota sen sijaan, että jakolasku ja jakojäännös olisivat suoraan tulostussilmukassa.

Erottelu on hyödyllinen, koska numeron valinta on determinististä aritmetiikkaa, kun taas `console_putc` ja `console_puts` vaikuttavat laitteistoon. Testi voi tarkistaa ensin mainitun ilman sarjapäätettä tai VGA-puskuria. Tämä ei vielä ole isännässä ajettava yksikkötestiohjelma: `dectest`-komento suorittaa tarkistukset käynnistetyssä kernelissä.

## Suorita itsetesti

Kirjoita VNTOSin komentokehotteeseen:

```text
dectest
```

Odotettu tuloste:

```text
Decimal digit self-test: PASS
```

Testi kattaa nollan, kymmenpotenssin desimaalipaikan, luvun sisäisen nollan, ykkösten numeron, nelinumeroisen luvun ensimmäisen numeron, nollajakajan ja negatiivisen luvun. QEMU-työnkulku lähettää komennon emuloidun näppäimistön kautta ja vaatii PASS-rivin sarjaportin tulosteessa.

Jos jokin tarkistus epäonnistuu, tuloste on `Decimal digit self-test: FAIL`. CI tarkistaa PASS-rivin, joten FAIL tai kaatuminen ennen odotettua tulostetta kaataa työnkulun. Tulostetarkistus todistaa, että testi ilmoitti onnistumisesta kyseisessä ajossa; se ei todista fyysisen laitteiston yhteensopivuutta eikä kaikkien 64-bittisten syötteiden toimivuutta.

## Rajat ja jatkokehitys

- `console_decimal` ei vieläkään voi turvallisesti ottaa vastalukua pienimmästä etumerkillisestä 64-bittisestä kokonaisluvusta. Älä käsittele sitä tuettuna syötteenä.
- Apufunktio olettaa jakajan olevan kymmenen potenssi; tuleva rajapinta voisi tarkistaa ehdon tai käyttää desimaalipaikan indeksiä.
- Koska `dectest` suoritetaan kernelissä, se on riippumaton konsolitulostuksesta vain aritmeettisen apufunktion tasolla. Erillinen isäntätesti edellyttäisi tuettua tapaa kääntää ja suorittaa sama VNT-apufunktio kernelin ulkopuolella.
- Pidä diagnostiikkatestit deterministisinä. Vältä testejä, jotka muuttavat varaajan tilaa, APIC-reititystä tai jaettua SMP-tilaa, ellei aiempaa tilaa voida palauttaa.

## Harjoitukset

1. Laske `decimal_digit_at(120304, 1000)`. Mikä numero valitaan?
2. Miksi `decimal_digit_at(907, 10)` palauttaa nollan eikä ohita sitä seuraavaan nollasta poikkeavaan numeroon?
3. Selitä, miksi nollajakaja pitää hylätä ennen jakolaskua.
4. Lisää testiin kaksinumeroinen luku ja jakaja 1.
5. Ehdota turvallista tapaa tulostaa pienin etumerkillinen 64-bittinen kokonaisluku ilman ylivuotoa vastalukua laskettaessa.

## Hyväksymiskriteerit

- [ ] Numeron poiminta on erillinen apufunktio ilman konsolikutsuja.
- [ ] Negatiiviset luvut ja ei-positiiviset jakajat palauttavat dokumentoidun virhearvon.
- [ ] Desimaalimuotoilija käyttää apufunktiota numeron valintaan.
- [ ] `dectest` kattaa edustavia numeroita ja virheellisiä syötteitä.
- [ ] QEMU CI vaatii itsetestin PASS-tulosteen.
- [ ] Dokumentaatio kertoo kymmenpotenssioletuksen ja pienimmän kokonaisluvun rajoituksen.

---

[← Edellinen oppitunti](./36-shell-help-output.md) · [Kurssin sisällysluettelo](./README.md)
