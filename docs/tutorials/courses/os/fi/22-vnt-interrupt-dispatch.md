# Käsittele keskeytyksiä VNT-kielellä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kun matalan tason sisääntulo on tallentanut tarvittavan tilan, VNT-koodi voi käsitellä tapahtuman merkityksen. Se voi tarkistaa vektorin, päivittää laskurin tai ohjata tapahtuman oikealle käsittelijälle.

## Sanasto

Dispatch tarkoittaa tapahtuman ohjaamista käsittelijälle. Tunnetulla vektorilla on määritelty toiminta. Tuntemattomalle tapahtumalle tarvitaan turvallinen oletuskäytös.

## Käytännön harjoitus

Avaa os/interrupts.vnt. Etsi kohta, jossa vektori tarkistetaan tai tapahtuma ohjataan. Seuraa yhden vektorin reittiä ja vertaa sitä os/arch/x86_64/interrupts.S-tiedostoon.

## Yleinen virhe ja turvallisuus

Keskeytyskontekstissa ei voi huolettomasti kutsua mitä tahansa funktiota. Lukkojen ja muistinhallinnan on noudatettava määriteltyä keskeytyskäytäntöä.

## Tarkista ymmärtäminen

Miksi sisääntulo ja tapahtuman käsittelypolitiikka erotetaan? Miten tuntemattomaan vektoriin pitäisi suhtautua?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Käsittelijän pitää tietää, mitä vektori tarkoittaa ja mitä tietoa se voi käyttää. Tuntematonta vektoria ei pidä hiljaisesti käsitellä tunnettuna tapahtumana. Pidä käsittelijä lyhyenä ja testattavana.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Pidä assembly-rajapinta pienenä](21-minimal-assembly-entry-stubs.md) · [Seuraava oppitunti: PIT, PIC ja ajastimen tikit →](23-pit-pic-and-timer-ticks.md)

## Toteutuslaboratorio: Keskeytysten VNT-dispatcher

Tämä harjoitus edellyttää lähdekoodin lukemista ja toteutuksen todentamista.

**Tutkittavat tiedostot:** `os/interrupts.vnt`, `os/kernel/device_irq.vnt`, `os/kernel/smp.vnt`

**Tavoite:** Pidä keskeytyksen päätöksenteko VNT:ssä ja käsittelijä rajattuna. Tunnista spurious-vector, IPI, legacy IRQ ja CPU-poikkeus erikseen. Älä tee raskasta työtä keskeytyskontekstissa.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Tarkista dispatcherista haarat vektoreille 255, 240, 32–47 ja 0–31.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

2. **Etsi nykyinen toteutus:** Varmista, että jokainen haara palautuu oikealla tavalla tai pysäyttää virhetilanteessa.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

3. **Kirjaa sopimus:** Tarkista missä PIC EOI lähetetään, jotta sitä ei tehdä kahdesti.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

4. **Tee pienin muutos:** Lisää uusi vektoritoiminto vain kun IDT ja assembly-stub ovat olemassa.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

5. **Tarkista riippuvuudet:** Testaa vektorien rajat 31/32 ja 47/48 sekä spurious-vektori.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

6. **Käännä:** Varmista, ettei dispatcher kutsu tulostusta tai allokaattoria tavallisessa IRQ-polussa.
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
