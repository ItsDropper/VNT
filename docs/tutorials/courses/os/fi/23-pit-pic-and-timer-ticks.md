# PIT, PIC ja ajastimen tikit

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

PIT on perinteinen ohjelmoitava ajastin ja PIC vanha keskeytysohjain. Ajastin voi tuottaa säännöllisiä tikkejä. Kernel voi laskea niiden avulla aikaa tai myöhemmin päättää, milloin tehtävä vaihdetaan. Tikki ei itsessään ole tehtäväajastin.

## Sanasto

IRQ on laitteistokeskeytyspyyntö. EOI kertoo keskeytysohjaimelle, että keskeytys on käsitelty. Tick on ajastintapahtuma. Scheduler valitsee, mikä tehtävä saa CPU:n.

## Käytännön harjoitus

Etsi ajastimen ja PICin alustus sekä tikin käsittelijä os/-hakemistosta. Seuraa tapahtumaketjua: IRQ, IDT, käsittelijä, tilan päivitys ja kuittaus. Tarkista lähdekoodista, mitä VNTOS oikeasti tekee.

## Yleinen virhe ja turvallisuus

Täysi scheduler tarvitsee tehtävärakenteet, rekisteritilan vaihdon, pinojen hallinnan ja selkeän keskeytyskäytännön. Tikin laskeminen ei vielä vaihda tehtäviä.

## Tarkista ymmärtäminen

Mitä tikki kertoo? Mitä scheduler tarvitsee ajastimen lisäksi?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Ajastintikki on tapahtuma, ei automaattisesti valmis ajanmittaus tai tehtävänvaihto. Selvitä, kuka päivittää laskurin, kuka kuittaa keskeytyksen ja mitä tapahtuu toistuvissa tapahtumissa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Käsittele keskeytyksiä VNT-kielellä](22-vnt-interrupt-dispatch.md) · [Seuraava oppitunti: Rakenna ja testaa keskeytyskernel →](24-build-and-test-the-interrupt-kernel.md)

## Koodiesimerkki: PIT-tikki ei ole vielä scheduler

```vnt
fun kernel_timer_tick() {
    state_set(11, state_get(11) + 1)
    return 0
}

fun kernel_timer_ticks() -> int {
    return state_get(11)
}
```

Tämä on nykyisen os/kernel/core.vnt-tiedoston nimetty rajapinta. IRQ0:n käsittely kasvattaa laskuria; komentotulkki voi lukea sen myöhemmin. Laskuri kertoo tikkejä, ei tarkkaa seinäkelloaikaa eikä sitä, että prosessien ajastus olisi toteutettu.

**Tee itse:** Käynnistä QEMU, suorita `irq`-komento kahdesti ja vertaa tikkejä. Erota toisistaan PIT:n ohjelmointi, PIC-maskit, IRQ:n laskenta ja scheduler; nykyinen laskuri toteuttaa vain tikin kirjaamisen.


## Toteutuslaboratorio: PIT, PIC ja ajastimen tikit

Tämä harjoitus edellyttää lähdekoodin lukemista ja toteutuksen todentamista.

**Tutkittavat tiedostot:** `os/arch/x86_64/interrupts.S`, `os/pic.vnt`, `os/interrupts.vnt`

**Tavoite:** Toteuta ajastimen tikin laskenta ilman pitkää työtä IRQ-polussa. Maski, reititys ja EOI ovat eri tehtäviä. Tikki ei itsessään tarkoita scheduleria.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Etsi PIC:n remappaus ja varmista vektorit 32–47.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

2. **Etsi nykyinen toteutus:** Tarkista master- ja slave-maskit sekä IRQ2:n kaskadivaatimus.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

3. **Kirjaa sopimus:** Etsi PIT:n jakaja ja laske odotettu taajuus dokumentoidusta lähtötaajuudesta.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

4. **Tee pienin muutos:** Tarkista, että IRQ0 kasvattaa laskuria eikä tulosta keskeytyksen sisällä.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

5. **Tarkista riippuvuudet:** Testaa maskattuna ja vain tarvittava IRQ avattuna QEMU:ssa.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

6. **Käännä:** Älä väitä tikin todistavan prosessien preemptiivista ajoitusta.
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
