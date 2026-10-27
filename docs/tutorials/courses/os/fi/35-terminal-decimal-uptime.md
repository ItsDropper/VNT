# Tulosta käyttöaika ja laskurit luettavina desimaalilukuina

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Miksi tällä on merkitystä tekstipohjaisessa kernelissä

Diagnostiikkakomento on hyödyllinen vain, jos sen tuloste on luettavaa ja yksiköt ovat selviä. Komento `uptime` tulosti aiemmin ajastimen tikit ja arvioidut sekunnit `console_hex`-toiminnolla, joten esimerkiksi 100 sekuntia näkyi muodossa `0x64`. Heksadesimaali sopii osoitteisiin ja rekisteriarvoihin, mutta ei ole luontevin oletus tavallisille tilatiedoille.

Tässä oppitunnissa lisätään pieni, muistia varaamaton `console_decimal(value)`-muotoilija ja otetaan se käyttöön ajastinpohjaisessa käyttöaikaraportissa. Tämä ei lisää reaaliaikakelloa eikä paranna PIT-ajastimen tarkkuutta.

## Desimaalimuotoilija

Toteutus: `os/console.vnt`.

```vnt
fun console_decimal(value: i64) {
    if value == 0 {
        console_putc(48)
        console_puts("\r\n")
        return 0
    }
    if value < 0 {
        console_putc(45)
        value = 0 - value
    }
    let divisor: i64 = 1
    while divisor <= value / 10 {
        divisor = divisor * 10
    }
    while divisor > 0 {
        let digit: int = value / divisor
        console_putc(48 + digit)
        value = value % divisor
        divisor = divisor / 10
    }
    console_puts("\r\n")
    return 0
}
```

Funktio tulostaa ASCII-numerot suoraan: merkkiarvo 48 on `0`, ja `48 + digit` tuottaa vastaavan merkin numeroille nollasta yhdeksään. Se ei varaa väliaikaista merkkijonoa eikä käytä kekomuistia. Tämä on tärkeää, koska VNTOSissa ei ole yleiskäyttöistä kekomuistin varaajaa.

### Miksi jakaja toimii

Positiivisella syötteellä ensimmäinen silmukka etsii suurimman kymmenen potenssin, joka ei ylitä lukua. Luvulla `1234` jakajaksi tulee `1000`. Jokaisella kierroksella tulostetaan ensimmäinen numero, poistetaan se jakojäännöksellä ja jaetaan jakaja kymmenellä:

| Vaihe | Arvo ennen | Jakaja | Tulostettu numero | Jäännös |
| --- | ---: | ---: | ---: | ---: |
| 1 | 1234 | 1000 | 1 | 234 |
| 2 | 234 | 100 | 2 | 34 |
| 3 | 34 | 10 | 3 | 4 |
| 4 | 4 | 1 | 4 | 0 |

Ehto `divisor <= value / 10` estää jakajan kertomisen syötteen suuruusluokan yli. Nolla tarvitsee oman haaransa, koska muuten numerotulostuksen silmukka ei käynnistyisi.

Nykyinen toteutus on tarkoitettu tavallisille etumerkillisille laskuriarvoille, ei pienimmälle mahdolliselle 64-bittiselle etumerkilliselle kokonaisluvulle: sen vastaluku ei mahdu samaan tyyppiin. Käyttöajan tikit ovat ei-negatiivisia, joten tämä poikkeus ei kuulu komennon odotettuun syötteeseen.

## Desimaali käyttöajalle, heksadesimaali osoitteille

Toteutus: `os/kernel/shell.vnt`.

Komento `uptime` tulostaa nyt:

- ajastimen tikit desimaalilukuna;
- arvioidut kuluneet sekunnit desimaalilukuna;
- huomautuksen siitä, että kellona käytetään nimellisesti 100 Hz:n PIT-tikkejä ja sekunnit ovat arvio.

Kaava on `seconds = ticks / 100`. Kokonaislukujako katkaisee murto-osan. PIT-taajuutta ei kalibroida reaaliaikakelloon, HPETiin tai invarianttiin TSC-laskuriin. Älä esitä tätä seinäkellona tai tarkkana kestoarvona.

Käytä `console_hex`-toimintoa osoitteille, maskeille, APIC-tunnisteille ja rekisteriarvoille, joissa heksadesimaali on tavanomainen esitystapa. Käytä desimaalia laskureille, joita käyttäjä lukee määrinä. Sisällytä yksikkö tulosteen otsikkoon, älä jätä sitä käyttäjän arvattavaksi.

## Todentaminen

Käännä ja käynnistä kernel projektin tuetulla työnkululla ja suorita:

```text
uptime
mem
irq
```

Odotettu tulosteen muoto (arvot vaihtelevat):

```text
Timer ticks (decimal):
Approximate uptime seconds (decimal):
Clock source: PIT at nominal 100 Hz; seconds are approximate.
```

Kahden ensimmäisen otsikon jälkeen tulee olla desimaalilukuja ilman `0x`-etuliitettä. `mem`-komennon tulee edelleen käyttää heksadesimaalia osoitteille ja nykyisille varaajan laskureille, kunnes niiden esitystapaa muutetaan erikseen. `irq`-komento ei muutu tässä oppitunnissa.

Hyödyllisiä muotoilijan testitapauksia ovat nolla, yksinumeroinen luku, kymmenen potenssi, sisäisiä nollia sisältävä luku ja suuri positiivinen arvo. Myöhemmässä muutoksessa muotoilu kannattaa erottaa puhtaaksi numeropuskuria käyttäväksi apufunktioksi, jota voidaan testata ilman laitteistotulostusta.

## Harjoitukset

1. Seuraa jakajaa ja jakojäännöstä luvulla `907`. Miksi ensimmäinen tulostettu numero ei saa olla nolla?
2. Mikä virhe syntyy, jos nollan erikoiskäsittely poistetaan?
3. Miksi `ticks / 100` hukkaa alle sekunnin tiedon?
4. Mitkä komentotulkin arvot kannattaa pitää heksadesimaalisina ja miksi?
5. Suunnittele testirajapinta, jolla CI voi tarkistaa nollan, 10:n, 100:n ja 907:n tulosteen ilman kernelin käynnistystä.
6. Miksi muotoilijan ei pidä varata kekomuistipuskuria?

## Hyväksymiskriteerit

- [ ] Desimaalitulostus käsittelee nollan ja tavalliset positiiviset arvot.
- [ ] Muotoilija ei varaa muistia eikä käytä libc-riippuvuutta.
- [ ] Käyttöajan tikit ja arvioidut sekunnit tulostetaan desimaalilukuina.
- [ ] Tuloste kertoo PIT-pohjaisesta arviosta ja sen rajoituksesta.
- [ ] Osoitteet ja rekisteridiagnostiikka voivat edelleen käyttää heksadesimaalia.
- [ ] PIT-tikkejä ei väitetä tarkaksi seinäkelloksi.
- [ ] Suomen- ja englanninkielinen oppitunti kuvaavat saman toteutuksen ja rajoitukset.

---

[← Edellinen oppitunti](./34-terminal-editable-line-editor.md) · [Sisällysluettelo](./README.md) · **Seuraavaksi: lisää deterministiset muotoilijatestit.**
