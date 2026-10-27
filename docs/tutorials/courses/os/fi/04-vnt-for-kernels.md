# VNT-kielen perusteet kernel-koodissa

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

VNTOSin tavallinen logiikka pyritään pitämään VNT-kielessä. Assemblya tarvitaan CPU:n tarkkoihin rajapintoihin, kuten käynnistykseen ja keskeytyksen sisääntuloon. Muuttujat säilyttävät arvoja, funktiot nimeävät toimintoja, ehdot valitsevat haaroja ja silmukat toistavat toimintoja.

## Sanasto

Osoitin viittaa muistiosoitteeseen. Freestanding tarkoittaa, ettei ohjelma voi olettaa tavallisen käyttöjärjestelmän palveluita. Funktiokutsu siirtää suorituksen toiseen funktioon ja palaa yleensä takaisin.

## Käytännön harjoitus

Avaa os/kernel.vnt ja etsi alustuksen pääpolku. Seuraa funktiokutsuja. Avaa sitten os/kernel/shell.vnt ja valitse yksi komentoa käsittelevä funktio. Kirjoita ylös, mitä se saa syötteeksi ja mitä se muuttaa.

## Yleinen virhe ja turvallisuus

VNT-kääntäjän tavallisille ohjelmille tarjoamat ominaisuudet eivät automaattisesti toimi freestanding-kernelissä. Virheellinen osoitin voi kaataa koko järjestelmän.

## Tarkista ymmärtäminen

Valitse yksi funktio ja selitä sen syöte, toiminta, kutsumat funktiot ja mahdollinen virhetilanne.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Seuraa tietoa funktiosta toiseen: argumentit ovat sisään tulevaa tietoa ja paluuarvo on tulos. Tarkista myös sivuvaikutukset eli muistipaikat tai laitteet, joita funktio muuttaa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---


## Koodiesimerkki: VNT-funktion rakenne ja rajat

```vnt
fun device_irq_count(irq: int) -> int {
    if irq < 0 || irq > 15 {
        return -1
    }
    return state_get(12 + irq)
}
```

Tämä on nykyisen os/kernel/device_irq.vnt-tiedoston funktio. `fun` määrittelee funktion, `-> int` paluuarvon tyypin, `if` tarkistaa rajan ja `return` päättää funktion. IRQ-numero tarkistetaan ennen kuin sillä lasketaan jaetun tilan indeksiä.

**Tee itse:** Kopioi rakenne omalle harjoitusfunktiollesi mutta vaihda nimi, parametrit ja rajat tarpeen mukaan. Määritä ensin virhetilan paluuarvo ja tarkista, miten kutsuja käsittelee sen. Älä käytä unsafe-lohkoa, ellei muistia tai laitteistoa todella tarvitse käsitellä.


## Toteutuslaboratorio: VNT kernelissä

Tämä osio täydentää käsitteitä käytännön toteutusmenettelyllä. Lähdekoodiviitteet ovat projektin nykyisiä polkuja; avaa ne ennen muutosta.

**Tutkittavat tiedostot:** `os/kernel.vnt`, `os/serial.vnt`, `os/interrupts.vnt`

**Tavoite:** Toteuta VNT-funktio, jonka sopimus on tarkka. Pidä unsafe-alue pieninä ja tarkistusten ympäröiminä. Varmista että kääntäjän tuottama symboli vastaa kutsujan odotusta.

### Tee työ järjestyksessä

1. **Rajaa vastuu:** Valitse yksi olemassa oleva VNT-funktio ja selvitä parametrit ja paluuarvo.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

2. **Etsi nykyinen toteutus:** Tee ensin vain aritmetiikkaa tai tilan lukua, ei laitteistokirjoitusta.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

3. **Kirjaa sopimus:** Käännä freestanding-assemblyksi ja tarkista generoitu symboli.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

4. **Tee pienin muutos:** Etsi, miten VNT-kääntäjä muuttaa funktiokutsun ABI-kutsuksi.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

5. **Tarkista riippuvuudet:** Jos käytät inline assemblyä, tarkista rekisterit, operandit ja pinon offsetit.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

6. **Käännä:** Testaa nolla, rajatapaus ja tavallinen arvo.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

### Selvitä jokainen rajapinta

- Mikä funktio aloittaa toiminnon, ja kuka sitä kutsuu?
- Mitä arvoja funktio lukee, mitä se muuttaa ja mitä se palauttaa?
- Mitkä muistiosoitteet, indeksit tai laitearvot tulevat ulkopuolelta?
- Mikä tarkistus tapahtuu ennen kuin arvoa käytetään?
- Mikä on selkeä virhetilan paluuarvo tai diagnostiikka?
- Mikä osa on VNT:tä ja mikä osa on CPU:n vaatimaa assemblyä?
- Mitä nykyinen toteutus nimenomaisesti ei tue?

### Hyväksymistesti

1. Kirjoita ennen käännöstä odotettu tuloste tai tilamuutos.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Käynnistä syntynyt kernel vain QEMU:ssa.
4. Tarkista sarjaportin tai konsolin tuloste ja vertaa sitä odotukseen.
5. Toista testi rajatapauksella ja virheellisellä syötteellä.
6. Jos et voi suorittaa testiä, ilmoita se avoimesti äläkä merkitse sitä läpäistyksi.

### Yleiset toteutusvirheet

- Uuden funktion lisääminen ilman kutsuja tai importtia: koodi ei ole osa käynnistyspolkua.
- Oletus, että käännös tarkoittaa käynnistymistä.
- Rajojen tarkistaminen vasta sen jälkeen, kun muistia on jo luettu tai kirjoitettu.
- Liian suuri muutos, jonka jälkeen ei tiedetä, mikä rikkoi toiminnan.
- Diagnostiikan lisääminen keskeytyskäsittelijään niin, että alkuperäinen virhe muuttuu uudeksi jumiutumiseksi.

### Muutosraportti

Kirjoita muistiin: (1) tiedostot, joita muutit, (2) uuden toiminnon sopimus, (3) tarkka build-tulos, (4) QEMU-havainto, (5) testatut virhepolut ja (6) rajoitukset, joita et ratkaissut. Tämä raportti on osa toteutusta, ei ylimääräistä paperityötä.

---

[← Edellinen oppitunti](./03-setup.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./05-build-pipeline.md)
