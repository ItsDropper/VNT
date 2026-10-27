# Paranna sivukehysten varaajaa next-fit-haulla

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite

Muuta fyysisen sivukehysvaraajan hakua niin, ettei jokainen varaus aloita etsintää uudelleen 1 MiB:n rajalta. Säilytä rajattu haku, oikeat laskurit ja testaa uuden hakukursorin toiminta. Muutos tehdään tiedostoon `os/mm/phys.vnt`. Kursorin tallennuspaikka on käynnistyskoodin yhteisessä tilataulukossa.

## Mitä varaaja hallitsee?

Nykyinen varaaja toimii käynnistyksen identiteettikartoituksen ensimmäisessä gigatavussa. Yksi sivukehys on 4096 tavua. Käyttökelpoisuusbittikartta kertoo, että firmware ilmoitti kehyksen käyttökelpoiseksi. Varattujen bittikartta kertoo, että ydin omistaa sen. Pysyvästi varattujen bittikartta estää vapauttamasta esimerkiksi kernelin ja sivutaulujen muistia.

`phys_alloc_page()` palauttaa joko 4096 tavuun kohdistetun fyysisen osoitteen tai nollan. Onnistunut varaus muuttaa yhden käyttökelpoisen ja vapaan kehyksen varatuksi ja vähentää vapaan muistin laskuria yhdellä. `phys_free_page()` hylkää virheellisen kohdistuksen, tuetun alueen ulkopuoliset osoitteet, jo varaamattomat tai ei-käyttökelpoiset kehykset, pysyvästi varatut kehykset ja kaksoisvapautukset.

## Miksi next-fit?

Vanha silmukka aloitti jokaisen haun kehyksestä 256 eli 1 MiB:n kohdalta. Jos matalat kehykset oli jo varattu, jokainen uusi pyyntö luki samoja bittikarttojen osia uudelleen. Next-fit muistaa viimeisen onnistuneen varauksen jälkeisen kehyksen, etsii siitä ylöspäin ja kiertää tarvittaessa kerran takaisin kehykseen 256. Tavallisessa peräkkäisten varausten tapauksessa tämä vähentää toistuvaa skannausta muuttamatta varaajan julkista toimintatapaa.

Next-fit on optimointi, ei rinnakkaisuuden hallintamekanismi. Varaaja käyttää edelleen tavallisia bittikartan luku- ja kirjoitusoperaatioita sekä yhteistä vapaan muistin laskuria. Ennen kuin kääntäjä ja kernel tarjoavat oikean atomisuus- ja muistijärjestyssopimuksen, varaajaa saa käyttää vain nykyisissä sarjoitetuissa käynnistys- ja komentotulkkipoluissa. Älä kutsu sitä samanaikaisesti AP-ytimiltä tai keskeytyskäsittelijöistä.

## Lähdekoodikartta

- `os/mm/phys.vnt`: muistialueiden tulkinta, bittikartat, varaus, vapautus ja `phys_self_test()`.
- `os/arch/x86_64/boot.S`: yhteinen tilataulukko; paikka 176 tallentaa seuraavan haettavan kehysindeksin.
- `os/kernel/core.vnt`: käynnistyskoodin tilataulukon luku- ja kirjoitusrajapinta.
- `os/kernel/shell.vnt`: `memtest`-komento, joka näyttää itsekoetestin tuloksen.
- `os/build.sh`: todellinen luettelo koottavista assembly-tiedostoista ja linkityksen syötteistä.

## Seuraa toteutusta

Aloita funktiosta `phys_init()`. Se tarkistaa Multiboot2-muistikartan, merkitsee käyttökelpoiset kehykset, varaa kernelin kuvan ja käynnistystietolohkon, laskee käyttökelpoiset ja vapaat kehykset sekä asettaa hakukursorin arvoon 256. Kursori on 4096-tavuisen kehyksen indeksi, ei tavumääräinen osoite.

Seuraavaksi lue `phys_alloc_try_frame(frame)`. Funktio hylkää indeksin, joka ei ole sallitulla alueella, tarkistaa käyttökelpoisuus- ja varausbitit, merkitsee kehyksen varatuksi, vähentää vapaan kehysmäärän, siirtää kursoria ja palauttaa arvon `frame * 4096`. Nollapalaute tarkoittaa, ettei tätä ehdokasta varattu.

Lopuksi seuraa `phys_alloc_page()`-funktiota. Se tarkistaa kursorin, käy ensin läpi kursorista alueen loppuun ja kiertää sitten kehyksestä 256 alkuperäistä kursoria edeltävään kehykseen asti. Jokainen ehdokas tarkistetaan enintään kerran yhden pyynnön aikana. Jos sopivaa kehystä ei löydy, funktio palauttaa nollan muuttamatta vapaiden kehysten laskuria.

## Koodiesimerkki: rajattu next-fit-haku

```vnt
fun phys_alloc_page() -> i64 {
    let start: int = state_get(176)
    if start < 256 || start >= 262144 {
        start = 256
        state_set(176, start)
    }

    let frame: int = start
    while frame < 262144 {
        let page: i64 = phys_alloc_try_frame(frame)
        if page != 0 {
            return page
        }
        frame = frame + 1
    }

    frame = 256
    while frame < start {
        let page: i64 = phys_alloc_try_frame(frame)
        if page != 0 {
            return page
        }
        frame = frame + 1
    }
    return 0
}
```

Yläraja 262144 saadaan jakamalla 1 GiB luvulla 4096 tavua. Kehys 256 on ensimmäinen sallittu kehys, koska alempi muisti on varattu. Toinen silmukka toteuttaa kierron: ehto `frame < start` estää tutkimasta ensimmäisen silmukan ehdokkaita uudelleen. Apufunktio siirtää paikan 176 kursorin vasta onnistuneen varauksen jälkeen.

## Tilataulukon sopimus

Paikka 176 lisätään `boot.S`-tiedoston nykyisten SMP-tilapaikkojen jälkeen. Sen arvo on kehysindeksi, ja kelvollinen alue on 256–262143. Nolla tai alueen ulkopuolinen arvo tulkitaan alustamattomaksi ja korjataan arvoon 256. `phys_init()` asettaa kursorin uudelleen alkuarvoon vapaan muistin laskemisen jälkeen.

Älä käytä paikkaa 27: paikat 12–27 ovat jo kuudentoista legacy-IRQ:n laskureita. Paikat 28 ja 29 sisältävät käyttökelpoisten ja vapaiden kehysten määrät, ja paikat 30 eteenpäin on jo varattu ACPI-, APIC- ja SMP-tilalle. Uuden paikan dokumentointi estää kahden alijärjestelmän hiljaisen päällekkäisyyden.

## Vapautus ja kursorin toiminta

Kun kehys vapautetaan onnistuneesti, vapaiden kehysten määrä kasvaa täsmälleen yhdellä. Jos vapautettu kehys sijaitsee nykyistä kursoria alempana, kursori siirretään siihen. Näin seuraava varaus voi käyttää vapautunutta matalaa osoitetta ilman, että pitää odottaa koko kierrosta. Epäonnistunut vapautus ei saa muuttaa laskuria eikä kursoria.

Kursorin päivitys ei ole atominen. Se sopii vain nykyiseen sarjoitettuun käyttöön. Älä päättele, että kursorin lisääminen tekisi varaajasta SMP-turvallisen: bittikartat ja vapaan muistin laskuri olisivat edelleen alttiita kilpailutilanteille.

## Testaa invariantit

Suorita projektin juuressa:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Kirjoita VNTOSin komentotulkissa `memtest` ja sen jälkeen `mem`. Itsetestin pitäisi tulostaa `Physical allocator self-test: PASS`. Testi tarkistaa, että varaus siirtää paikan 176 seuraavaan kehykseen (tai takaisin arvoon 256 alueen lopussa), osoite on kohdistettu, kohdistamaton vapautus hylätään, kelvollinen vapautus onnistuu, toinen vapautus hylätään ja vapaiden kehysten laskuri palautuu alkuarvoon.

Jos `build/vntos.iso` ei syntynyt, älä suorita QEMU-komentoa sellaisenaan. Asenna ISO-työkalut `os/README.md`-ohjeen mukaan tai käytä olemassa olevaa `build/kernel.elf`-tarkistuspolkua. Pelkkä käännöksen onnistuminen ei todista kursorin toimintaa. Tarkista GitHub Actions -tulos täsmälleen samalta commitilta ennen kuin väität CI:n läpäisseen.

## Virhetilanteet

1. **Kursori on nolla:** varaaja asettaa sen kehykseen 256 ennen hakua.
2. **Kursori on vähintään 262144:** arvo on virheellinen ja korjataan kehykseen 256.
3. **Kursorin yläpuolella ei ole vapaita kehyksiä:** haku kiertää ja tarkistaa alemman alueen yhden kerran.
4. **Kaikki kehykset on varattu:** molemmat silmukat päättyvät ja palauttavat nollan; vapaan muistin laskuri ei muutu.
5. **Kehys vapautetaan kursorin alapuolelta:** kursori siirtyy taaksepäin, jotta kehys voidaan löytää seuraavassa haussa.
6. **Pysyvästi varattu kehys vapautetaan:** `phys_free_page()` hylkää pyynnön ennen laskurien muuttamista.
7. **Kaksi CPU:ta varaa yhtä aikaa:** tämä ei ole tuettu; molemmat voivat nähdä saman vapaan bitin. Älä anna AP-ytimien käyttää yleistä varaajaa.

## Toteutusharjoitus

1. Seuraa vapaan muistin laskuria yhden varauksen ja yhden vapautuksen ajan.
2. Piirrä kursorin reitti, kun `start = 300`: ensin 300–262143 ja sitten kierto 256–299.
3. Selitä, miksi toinen silmukka käyttää ehtoa `frame < start` eikä `frame <= start`.
4. Tarkista kaikki kernelin käyttämät tilapaikat ennen uuden paikan lisäämistä.
5. Laajenna `phys_self_test()`-testiä vain, jos lisätarkistus voidaan suorittaa ja tila palauttaa kaikissa poluissa.
6. Suunnittele tuleva SMP-turvallinen ratkaisu, mutta älä yritä toteuttaa sitä `volatile`-määreellä tai inline-assemblylla, jonka muistivaikutuksia kääntäjä ei tunne. Kääntäjä tarvitsee ensin tuetun atomisuus- ja muistijärjestysrajapinnan.

## Hyväksymiskriteerit

- [ ] `phys_init()` alustaa kursorin vasta, kun muistikartta on hyväksytty.
- [ ] Onnistunut varaus siirtää kursoria ja vähentää vapaiden kehysten laskuria kerran.
- [ ] Epäonnistunut varaus palauttaa nollan muuttamatta laskuria.
- [ ] Haku kiertää enintään kerran eikä tutki samaa ehdokasta kahdesti.
- [ ] Kelvollinen vapautus kasvattaa laskuria kerran ja muuttaa kursoria vain tarvittaessa.
- [ ] Virheellinen vapautus ja kaksoisvapautus eivät muuta varaajan tilaa.
- [ ] `memtest` läpäisee QEMU-testin ja todellinen tulos kirjataan.
- [ ] Sekä englanninkielinen että suomenkielinen kurssi linkittävät tähän oppituntiin.

## Nykyiset rajoitukset

Varaaja kattaa vain ensimmäisen 1 GiB:n identiteettikartoituksen, käyttää käynnistyskuvaan sijoitettuja bittikarttoja eikä sisällä yleistä heap-varaajaa tai SMP-lukkoa. Next-fit vähentää toistuvaa skannausta; se ei ratkaise pirstoutumista, usean peräkkäisen sivun varaamista, varattujen sivujen palauttamista tai rinnakkaista käyttöä.

---

[← Edellinen oppitunti](./29-smp-synchronization-contract.md) · [Sisällysluettelo](./README.md) · **Kurssi suoritettu.**
