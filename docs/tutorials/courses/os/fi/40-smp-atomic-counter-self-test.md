# Testaa atomisia lisäyksiä usealla CPU-ytimellä

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Muutetaan atomisen lisäyksen väite lähdekoodissa toistettavaksi moniydintestiksi. `atomictest`-komento käskee käynnistysprosessorin (BSP) ja jokaisen verkossa olevan apuprosessorin (AP) kasvattamaan samaa erillistä laskuria samanaikaisesti. Lopuksi tulosta verrataan täsmälliseen odotettuun kokonaismäärään.

Tämä on rajattu smoke-testi nykyiselle x86-64/QEMU-toteutukselle. Se ei ole muodollinen todistus kaikesta muistijärjestyksen toiminnasta eikä tee muusta ytimen tilasta SMP-turvallista.

## Miksi aiemmat testit eivät riittäneet?

Laskuri voi näyttää toimivan yhden CPU-ytimen testissä, vaikka päivitys koostuisi erillisistä luku-, lisäys- ja kirjoitusoperaatioista. Kaksi ydintä voi lukea saman arvon ja ylikirjoittaa toistensa lisäyksiä. `lock incq` -käskyn pitäisi estää tämä, mutta sen varmistamiseksi tarvitaan testi, jossa työ todella limittyy usealla ytimellä.

Älä testaa elävää ajastin- tai IRQ-laskuria, koska niihin tulee muutoksia muista syistä. Sen sijaan assemblyn määrittelemästä `vntos_state`-taulukosta varataan kolme omaa paikkaa:

- Paikka 308: jaettu atominen laskuri.
- Paikka 309: testin aktiivisuuslippu.
- Paikka 310: erän suorittaneiden AP-ytimien määrä.
- Paikka 311: aloitusesteellä odottavien AP-ytimien määrä.
- Paikka 312: aloitusesteen vapautuslippu.

Paikat lisätään komentohistorian jälkeen, joten aiemmat indeksit säilyvät.

## Testin suoritus

Toteutus on tiedostossa `os/kernel/smp.vnt`, ja käyttäjälle näkyvä komento tiedostossa `os/kernel/shell.vnt`.

1. Vaadi vähintään kaksi verkossa olevaa ydintä ja että kaikki löydetyt ytimet ovat verkossa. Muussa tapauksessa tulos on `SKIP`, ei `PASS`.
2. Nollaa oma laskuri ja valmistumisindikaattori sekä aktivoi testi.
3. Lähetä kiinteän vektorin IPI 240 (heksana `0xf0`) jokaiselle verkossa olevalle AP-ytimelle.
4. Kukin AP kasvattaa valmiuslaskuria atomisesti ja odottaa rajatulla aloitusesteellä.
5. BSP odottaa kaikkien AP-ytimien olevan valmiita, vapauttaa esteen ja suorittaa omat 1 000 lisäystään AP-ytimien kanssa. AP kasvattaa valmistumislaskuria atomisesti erän jälkeen.
6. Odota AP-erien valmistumista rajatulla spin-loopilla, poista aktiivisuuslippu ja vertaa tulosta arvoon `online_cpu_count * 1000`.

Ydintoiminto on tarkoituksella pieni:

```vnt
let work: int = 0
while work < 1000 {
    state_increment_atomic(308)
    work = work + 1
}
```

Kaikkien osallistujien on kasvatettava jaettua laskuria atomisesti. Tavallinen `state_set(308, state_get(308) + 1)` palauttaisi testattavan kilpailutilanteen.

## Suorita testi ja tulkitse tulos

Käynnistä ydin vähintään kahdella CPU-ytimellä. Projektin QEMU-työnkulku käyttää neljää:

```sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -smp 4 -serial stdio
```

Suorita VNTOS-kehotteessa:

```text
atomictest
```

Neljä verkossa olevaa ydintä tuottaa onnistumisriville 4 ydintä, 1 000 lisäystä ydintä kohti ja yhteensä 4 000. `FAIL` näyttää havaitun kokonaismäärän ja AP-valmistumisten määrän. `SKIP` tarkoittaa, etteivät ydinmäärän esiehdot täyty; se ei tarkoita, että primitiivi olisi läpäissyt testin.

CI-työnkulku suorittaa komennon neljän ytimen QEMUssa ja tarkistaa täsmällisen onnistumistulosteen. Pelkkä commit ei todista CI:n onnistuneen: tarkista kyseisen commitin GitHub Actions -ajo.

## Tärkeät rajoitukset

- Testi käyttää projektin nykyistä kääntäjäkohtaista inline-assemblyn kutsusopimusta. `state_increment_atomic` lukee ensimmäisen argumentin osoitteesta `-8(%rbp)`; freestanding-ABI:n muuttaminen edellyttää primitiivin uudelleenarviointia.
- Testi olettaa AP-ytimien käynnistyneen ja pystyvän vastaanottamaan olemassa olevan kiinteän vektorin IPI:n. Testi ohitetaan, jos kaikkia löydettyjä ytimiä ei ole saatu verkkoon.
- Testi käynnistetään komentotulkista BSP:llä normaalin keskeytyskäsittelyn ollessa käytössä. AP-käsittelijät tekevät rajatun työmäärän ja kuittaavat IPI:n erän jälkeen.
- Aktiivisuuslippu ja valmius-/vapautuseste ovat testikohtaista ohjaustilaa, eivät yleiskäyttöisiä synkronointiprimitiivejä. AP:n odotus on rajattu, jotta epäonnistunut este ei jätä ydintä pyörimään ikuisesti.
- Testin läpäisy ei todista fyysisen muistinvaraajan, komentohistorian, CPU-laskurin, spinlockin kilpailutilanteen tai mielivaltaisen jaetun datan olevan turvallista rinnakkaisessa käytössä.
- Aikakatkaisu tai epäonnistunut IPI on virhe, ei onnistunut testi pienemmällä tuloksella.

## Harjoitukset

1. Laske odotettu kokonaismäärä 2, 4 ja 8 verkossa olevalle ytimelle.
2. Selitä, miksi AP-valmistumisten laskuri on erillinen lisäyslaskurista.
3. Muuta eräkokoa vasta, kun olet jäljittänyt kaikki vakion käyttökohdat, ja päivitä CI-odotus.
4. Kuvaile, miten rikkinäinen lue–lisää–kirjoita-primitiivi voisi tuottaa odotettua pienemmän tuloksen.
5. Suunnittele jatkotesti, joka toistaa työkuorman monta kertaa käyttämättä samaa laskuria uudelleen ennen kuin aiempi AP-käsittelijä on varmasti valmis.

## Hyväksymiskriteerit

- [ ] Testi käyttää omia paikkoja eikä muuta ajastin- tai IRQ-laskureita.
- [ ] Jokainen CPU suorittaa täsmälleen 1 000 atomista lisäystä.
- [ ] BSP odottaa kaikkien AP-erien valmistumista rajatulla loopilla.
- [ ] Lopputulosta verrataan CPU-määrästä laskettuun täsmälliseen odotusarvoon.
- [ ] Testi ilmoittaa SKIP, jos CPU-topologia ei riitä.
- [ ] CI tarkistaa neljän ytimen odotetun tuloksen ja sen todellinen tulos tarkistetaan.

---

[← Edellinen oppitunti](./39-atomic-interrupt-counters.md) · [Kurssin sisällysluettelo](./README.md)
