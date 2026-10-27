# Pidä assembly-rajapinta pienenä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Assembly on tarpeen arkkitehtuurin rajakohdissa, mutta siitä ei pidä tehdä koko kernelin kieltä. Keskeytyksen sisääntulo voi tallentaa rekisterit ja valmistella tilan ennen VNT-funktiokutsua. Tapahtuman varsinainen käsittely kuuluu yleensä VNT-koodiin.

## Sanasto

Stub on pieni välityskoodi. Calling convention määrittää argumenttien, rekisterien ja paluuarvojen käytön. Clobber tarkoittaa rekisteriä tai tilaa, jonka koodi voi muuttaa.

## Käytännön harjoitus

Lue os/arch/x86_64/interrupts.S. Kirjaa, mitä tehdään ennen VNT-käsittelijää ja sen jälkeen. Selvitä pinon muutokset ja tallennetut rekisterit. Vertaa VNT-funktion odottamaan tietorakenteeseen.

## Yleinen virhe ja turvallisuus

Inline-assembly ilman kääntäjälle ilmoitettuja muistivaikutuksia voi johtaa virheelliseen koodiin. Käytä määriteltyä ja testattua kääntäjärajapintaa, älä irrallista assembly-tekstiä synkronoinnin korvikkeena.

## Tarkista ymmärtäminen

Nimeä yksi tehtävä, joka vaatii assemblya, ja yksi, joka kuuluu VNT-koodiin.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Kun arvioit, kuuluuko koodi assemblyyn, kysy vaatiiko se tietyn käskyn, rekisterin tai pinon. Jos kyse on yleisestä päätöksenteosta, VNT-koodi on yleensä helpompi ylläpitää.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Keskeytykset, poikkeukset ja IDT](20-interrupts-and-the-idt.md) · [Seuraava oppitunti: Käsittele keskeytyksiä VNT-kielellä →](22-vnt-interrupt-dispatch.md)

## Koodiesimerkki: tallenna rekisterit ennen VNT-kutsua

```asm
interrupt_common:
    cld
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
```

Tämä on os/arch/x86_64/interrupts.S:n common-entryn rekisterien tallennusjärjestyksen alkua. Keskeytys voi tapahtua kesken minkä tahansa kernel-koodin; siksi keskeytyksen käsittelijän on säilytettävä keskeytetyn koodin tila. Koko tiedostossa rekisterit palautetaan vastakkaisessa järjestyksessä ennen `iretq`-käskyä.

**Tee itse:** Piirrä pino jokaisen pushq-käskyn jälkeen. Kun luet palautuspolkua, varmista että jokainen tallennus saa vastinparin oikeassa järjestyksessä. Älä kopioi pelkkää katkelmaa korvaamaan koko yhteistä sisääntuloa.


## Toteutuslaboratorio: Pieni assembly-rajapinta

Tämä harjoitus edellyttää lähdekoodin lukemista ja toteutuksen todentamista.

**Tutkittavat tiedostot:** `os/arch/x86_64/interrupts.S`, `os/arch/x86_64/boot.S`

**Tavoite:** Pidä assemblyssä vain CPU:n ja ABI:n vaatima mekanismi. Älä siirrä kernelin politiikkaa assembly-makroihin. Dokumentoi pinon muoto tarkasti.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Etsi mikä osa vaatii käskyjä, joita VNT ei voi turvallisesti ilmaista.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

2. **Etsi nykyinen toteutus:** Määritä VNT:n odottama kehys ja järjestä pinon kentät täsmälleen siihen.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

3. **Kirjaa sopimus:** Varmista rekisterien tallennus ja palautus symmetrisesti.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

4. **Tee pienin muutos:** Tarkista stack alignment ennen jokaista ulkoista kutsua.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

5. **Tarkista riippuvuudet:** Varmista ettei red zone ole käytössä ja että käännös käyttää -mno-red-zone.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

6. **Käännä:** Testaa yksi stub ja vertaa pinon rakennetta debuggerilla tai lähdekoodin laskelmalla.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

### Rajapinta- ja turvallisuustarkistus

- Varmista, että vektori, pino, rekisterit ja paluuarvo ovat täsmälleen sovitussa muodossa.
- Tarkista kaikki rajat ennen kuin luet muistia tai kirjoitat laitteistorekisteriin.
- Erota CPU:n vaatima assembly VNT:n politiikasta.
- Määritä virhetilanne: diagnoosi, hallittu pysäytys tai turvallinen ohitus.
- Tarkista, ettei uusi haara tee työtä, joka voi odottaa keskeytyksen jälkeen.
- Dokumentoi ominaisuudet, joita tämä muutos ei vielä toteuta.

### Todennus

1. Kirjoita odotettu vektori ja tulos ennen muutosta.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut build-vaihe.
4. Tarkista symbolit ja linkitetyt objektit, jos muutat ABI:a.
5. Käynnistä QEMU:ssa ja tallenna sarjaportin tuloste.
6. Testaa normaali vektori, rajavektori ja virhepolku.
7. Älä ilmoita testausta läpäistyksi ilman havaintoa.

### Tavalliset virheet

- Väärä stub-tyyppi siirtää pinon kentät.
- Rekisteri jää tallentamatta tai palautetaan väärässä järjestyksessä.
- EOI puuttuu tai tehdään kahdesti.
- IDT-portti osoittaa väärään symboliin.
- Keskeytyskäsittelijä jää odottamaan laitetta tai tekee pitkän tulostuksen.

### Muutosraportti

Kirjaa muutos, rajapintasopimus, build-tulos, QEMU-havainto, testatut vektorit ja avoimet rajoitukset.
