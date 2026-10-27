# Käytä atomisia lisäyksiä jaetuissa keskeytyslaskureissa

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Miksi tavallinen lue–muuta–kirjoita ei riitä?

Laskurin päivitys muodossa `state_set(slot, state_get(slot) + 1)` koostuu erillisistä operaatioista: luku, lisäys ja kirjoitus. Jos kaksi CPU-ydintä suorittaa sarjan samaan aikaan, molemmat voivat lukea saman vanhan arvon ja kirjoittaa saman uuden arvon. Toinen tapahtuma katoaa laskurista.

Keskeytyskontekstissa on lisäksi vältettävä tavallisen spinlockin käyttöä: keskeytys voi osua koodiin, joka pitää samaa lukkoa, jolloin käsittelijä jää odottamaan lukkoa, jota keskeytetty koodi ei voi vapauttaa. Laskennan pitää olla lyhyt, eikä keskeytysdispatserissa pidä varata muistia tai tulostaa.

## Lisää rajattu atominen primitiivi

Tiedostoon `os/kernel/core.vnt` lisättiin `state_increment_atomic(index)`, joka suorittaa x86-64:n lukitun lisäyksen kohdistettuun `vntos_state`-taulukkoon:

```vnt
fun state_increment_atomic(index: int) {
    unsafe {
        asm volatile("leaq vntos_state(%rip), %rax; movq -8(%rbp), %rcx; lock incq (%rax,%rcx,8)")
    }
    return 0
}
```

`lock`-etuliite tekee muistipäivityksestä atomisen CPU-ydinten välillä. Koodi käyttää projektin nykyistä inline-assemblyn argumenttisopimusta, jossa ensimmäinen argumentti luetaan osoitteesta `-8(%rbp)`. Tämä on kääntäjäkohtainen sopimus, joka on tarkistettava uudelleen, jos freestanding-ABI muuttuu.

Tämä ei ole yleiskäyttöinen atominen rajapinta: se ottaa tilataulukon indeksin ja luottaa kutsujan käyttävän kelvollista paikkaa. Älä anna komentotulkin syötteen valita mielivaltaista indeksiä.

## Ota käyttöön ajastimessa ja IRQ-laskennassa

`kernel_timer_tick()` kasvattaa nyt paikkaa 11 primitiivin avulla. `device_irq_record(irq)` tarkistaa legacy IRQ -alueen 0–15 ja kasvattaa atomisesti paikkaa `12 + irq`. Virheellinen IRQ-numero palautetaan edelleen käsittelemättä muistia.

Keskeytysdispatseri tekee yhä vain välttämättömän työn: kirjaa vektorin ja kasvattaa ajastimen laskuria IRQ0:n yhteydessä. Se ei tulosta eikä suorita laitekäsittelijöitä. Komentotulkki lukee laskureista tilannekuvan. x86-64:ssä kohdistettu 64-bittinen luku on atominen, mutta näytetty arvo voi vanhentua heti lukemisen jälkeen.

## Mitä tämä takaa — ja mitä ei?

- Samanaikaiset atomiset lisäykset samaan paikkaan eivät enää ylikirjoita toisiaan.
- Tämä ei tee saman paikan kaikista käytöistä turvallisia, jos tuleva koodi käyttää tavallista lue–muuta–kirjoita-päivitystä.
- Tämä ei tee koko `vntos_state`-taulukosta, PS/2-ajureista, muistinvaraajasta tai laitteiden omistajuusmallista SMP-turvallista.
- Tämä ei todista IRQ-reititystä oikeaksi eikä takaa kaikkien laitetapahtumien saapumista dispatseriin.
- Nykyinen QEMU-tulostetesti tarkistaa tunnisteet, ei atomisen primitiivin moniydinkuormitusta.

## Todentamissuunnitelma

Projektin GitHub Actions -työnkulku on edelleen varsinainen build- ja QEMU-testi. Tarkista tämän muutoksen sisältävän commitin työnkulun tulos; älä päättele onnistumista pelkästä commitista. Tulevan erillisen testin pitäisi käyttää useita CPU-ytimiä ja omaa laskuria, synkronoida aloitus ja lopetus tulostamatta kuormituspolulla sekä verrata lopputulosta tarkkaan lisäysten määrään. Älä käytä elävää ajastin- tai IRQ-laskuria testiin.

## Harjoitukset

1. Esitä kahden CPU-ytimen suoritusjärjestys, jossa lue–lisää–kirjoita hukkaa lisäyksen.
2. Selitä, miksi keskeytyskäsittelijän spinlock voi aiheuttaa deadlockin.
3. Miksi laskurin lukema on vain tilannekuva, vaikka lisäykset ovat atomisia?
4. Mitä tarkistuksia yleiskäyttöinen `state_increment_atomic` tarvitsisi?
5. Suunnittele QEMU-kuormitustesti, joka laskee usean ytimen lisäykset ilman seinäkelloon perustuvaa ajoitusta.

## Hyväksymiskriteerit

- [ ] Ajastimen ja legacy IRQ -laskurit käyttävät atomisia lisäyksiä.
- [ ] IRQ-numero tarkistetaan ennen tilapaikan indeksin muodostamista.
- [ ] Keskeytysdispatseri ei varaa muistia eikä tulosta.
- [ ] Dokumentaatio rajaa väitteet tässä muutettuihin laskureihin.
- [ ] CI:n onnistumisesta kerrotaan vasta todellisen työnkulkutuloksen tarkistamisen jälkeen.

---

[← Edellinen oppitunti](./38-irq-controller-diagnostics.md) · [Kurssin sisällysluettelo](./README.md)
