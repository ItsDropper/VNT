# Peräkkäisten fyysisten sivujen varaaminen

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Lisää varaajaan rajapinta, joka varaa useita vierekkäisiä 4 KiB:n fyysisiä sivukehyksiä. Tällaista aluetta tarvitaan esimerkiksi joissakin DMA-puskureissa ja tulevissa kernelin muistipoolissa. Toteutus kuuluu tiedostoon `os/mm/phys.vnt`. Varaus tai vapautus ei saa jättää osittaista aluetta huomaamatta varatuksi.

## Rajapinnan sopimus

- `phys_alloc_contiguous_pages(count)` palauttaa ensimmäisen fyysisen osoitteen alueelle, jossa on täsmälleen `count` vierekkäistä sivua.
- Onnistunut varaus merkitsee kaikki kehykset varatuiksi, vähentää vapaiden sivujen laskuria määrällä `count` ja siirtää next-fit-kursorin alueen jälkeiseen kehykseen.
- Virheellinen määrä tai epäonnistunut haku palauttaa nollan muuttamatta varausbittejä tai laskureita.
- `phys_free_contiguous_pages(address, count)` tarkistaa koko alueen ennen yhdenkään bitin nollaamista.
- Virheellinen osoite, määrä, varattu sivu, varaamaton sivu tai ensimmäisen 1 GiB:n alueen ylitys hylkää koko vapautuksen.

Rajapinta toimii edelleen vain yhden CPU:n sarjoitetussa käytössä. “Kaikki tai ei mitään” tarkoittaa yhden kutsun virhekäyttäytymistä; se ei tee varaajasta säieturvallista.

## Koodiesimerkki: tarkista koko alue ensin

```vnt
fun phys_range_is_free(first: int, count: int) -> int {
    if count <= 0 || first < 256 || first >= 262144 {
        return 0
    }
    if count > 262144 - first {
        return 0
    }

    let frame: int = first
    while frame < first + count {
        if phys_usable_get(frame) != 1 ||
           phys_allocated_get(frame) != 0 ||
           phys_reserved_get(frame) != 0 {
            return 0
        }
        frame = frame + 1
    }
    return 1
}
```

Kehys 256 vastaa 1 MiB:n rajaa. Kehys 262144 on ensimmäisen gigatavun ulkopuolinen loppuraja. Ehto `count > 262144 - first` tarkistaa rajan ennen kuin määrää lisätään kehysindeksiin. Jokaisen kehyksen on oltava käyttökelpoinen, varaamaton eikä pysyvästi varattu.

## Varausalgoritmi

`phys_alloc_contiguous_pages(count)` tarkistaa määrän, korjaa tarvittaessa next-fit-kursorin ja etsii ehdokkaan, jonka koko alue läpäisee `phys_range_is_free()`-tarkistuksen. Ensin kokeillaan kursorista ylöspäin kaikki aloituskohdat, joista koko alue mahtuu muistiin. Sen jälkeen haku kiertää kehykseen 256 ja tarkistaa alkuperäistä kursoria pienemmät aloituskohdat. Fyysinen alue ei koskaan saa kiertää kartoituksen lopusta alkuun.

Vasta kun koko ehdokas on hyväksytty, jokainen kehys merkitään varatuksi ja vapaiden sivujen laskuria vähennetään. Järjestys on tärkeä: jos ehdokas sisältää yhdenkin varatun tai käyttökelvottoman kehyksen, aiempia kehyksiä ei saa jättää varatuiksi.

## Vapautus: tarkista ensin, muuta vasta sitten

Vapautuksessa on kaksi silmukkaa tarkoituksella. Ensimmäinen tarkistaa kaikkien kehysten käyttökelpoisuuden, varausbitin ja pysyvän varauksen. Jos yksikin tarkistus epäonnistuu, funktio palaa ennen tilan muuttamista. Vasta toinen silmukka nollaa varausbitit. Sen jälkeen vapaan muistin laskuria kasvatetaan määrällä `count`, ja next-fit-kursori siirretään taaksepäin, jos vapautettu alue alkaa sen alapuolelta.

Tämä on pieni transaktiomalli: **tarkista kaikki ehdot ja tee vasta sitten muutokset**. Älä yhdistä tarkistusta ja muuttamista samaan silmukkaan, koska silloin vapautus voisi jäädä osittaiseksi.

## Esimerkkikutsu

```vnt
let run: i64 = phys_alloc_contiguous_pages(4)
if run == 0 {
    console_puts("Could not allocate four adjacent pages.\\r\\n")
} else {
    # run osoittaa ensimmäiseen sivuun; seuraavat ovat 4096 tavun välein.
    if phys_free_contiguous_pages(run, 4) != 1 {
        console_puts("Contiguous-page release failed.\\r\\n")
    }
}
```

Neljä sivua vie yhteensä 16384 tavua. Palautettu arvo on ensimmäisen sivun fyysinen osoite, ei osoitin yleisen heapin olioon. Nykyinen ensimmäisen gigatavun identiteettikartoitus tekee fyysisestä osoitteesta suoraan käytettävän tässä kernelissä, mutta tuleva sivutus ei saa olettaa fyysisten ja virtuaalisten osoitteiden olevan aina samoja.

## Itsetestin tarkistukset

Nykyinen komentotulkin `memtest` kutsuu `phys_self_test()`-funktiota. Testi varaa nyt myös kaksi vierekkäistä kehystä ja tarkistaa:

1. Varaus on sivun rajalle kohdistettu.
2. Vapaiden sivujen laskuri pienenee täsmälleen kahdella.
3. Molemmat kehykset merkitään varatuiksi.
4. Kohdistamaton vapautus hylätään eikä laskuri muutu.
5. Kelvollinen vapautus palauttaa laskurin.
6. Saman alueen toinen vapautus hylätään.
7. Lopullinen vapaan muistin määrä vastaa alkuarvoa.

Tämä on rajattu regressiotesti, ei todiste kaikista hakupoluista. Nykyinen itsetesti ei esimerkiksi rakenna täysin loppuun käytettyä muistikarttaa eikä pakota aluetta ylittämään viimeistä kehystä. Näihin tarvitaan testirajapinta tai erillinen varaajan testiohjelma.

## Testaa QEMUssa

Suorita projektin juuressa:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Aja komentotulkissa `memtest` ja sen jälkeen `mem`. Odotettu testiviesti on `Physical allocator self-test: PASS`. Käytä täsmälleen kyseisen commitin GitHub Actions -tulosta build- ja käynnistystodisteena; lähdekoodin lukeminen ei ole testitulos.

## Virhetilanteet

- **Määrä on nolla tai negatiivinen:** hylkää ennen bittikartan lukemista.
- **Määrä on liian suuri:** hylkää ennen alueen loppuindeksin laskemista.
- **Ehdokasalueessa on varattu kehys:** jatka hakua äläkä merkitse osaa alueesta.
- **Yhtenäistä aluetta ei löydy:** palauta nolla ja säilytä vapaan muistin laskuri.
- **Alue sisältää pysyvästi varatun kehyksen:** hylkää koko vapautus ja säilytä kaikki bitit.
- **Osoite ei ole sivun rajalla:** hylkää ennen kehysindeksin laskemista.
- **Alue ylittää kartoituksen:** hylkää se, älä katkaise aluetta hiljaisesti.
- **Kaksi CPU:ta kutsuu varaajaa yhtä aikaa:** ei tuettu; bittikartat ja kursori eivät ole atomisia.

## Toteutusharjoitus

1. Seuraa kolmen sivun varausta ja kirjaa laskurin arvo jokaisessa vaiheessa.
2. Selitä, miksi yhtenäinen varaus ei voi vain kutsua `phys_alloc_page()`-funktiota monta kertaa: myöhempi epäonnistuminen jättäisi osittaisen alueen varatuksi.
3. Selitä, miksi vapautuksen pitää tarkistaa koko alue ennen ensimmäisen bitin nollaamista.
4. Lisää testi määrälle `count == 0` ja varmista, ettei laskuri tai kursori muutu.
5. Suunnittele testirajapinta, jolla voi simuloida vapaata aluetta katkaisevan varatun kehyksen sotkematta käynnissä olevan kernelin varaajaa.
6. Pidä SMP-varaukset poissa käytöstä, kunnes kääntäjä tarjoaa dokumentoidun atomisen operaation ja bittikartan, laskurin sekä kursorin päivityksiä suojaa yhteinen lukko.

## Hyväksymiskriteerit

- [ ] Varaus hylkää virheelliset määrät ja tuetun alueen ylittävät alueet.
- [ ] Onnistunut varaus palauttaa vierekkäiset kehykset ja päivittää laskurin oikein.
- [ ] Epäonnistunut varaus ei muuta bittejä, laskuria eikä kursoria.
- [ ] Vapautus tarkistaa koko alueen ennen muutoksia.
- [ ] Virheelliset ja toistetut vapautukset eivät muuta osaa alueesta.
- [ ] `memtest` läpäisee QEMU-testin ja todellinen CI-tulos kirjataan.
- [ ] Sekä englannin- että suomenkielinen kurssi linkittää tähän oppituntiin.

## Nykyiset rajoitukset

Varaaja kattaa yhä vain ensimmäisen 1 GiB:n identiteettikartoituksen ja käyttää kernelin kuvaan tallennettuja bittikarttoja. Yhtenäisen alueen varaus auttaa tulevia alijärjestelmiä, mutta se ei toteuta yleistä heap-varaajaa, DMA-välimuistin yhtenäisyyspolitiikkaa, vapautettavia pysyviä varauksia, virtuaalimuistin eristystä eikä SMP-turvallista lukitusta.

---

[← Edellinen oppitunti](./30-physical-allocator-next-fit.md) · [Sisällysluettelo](./README.md) · **Kurssi suoritettu.**
