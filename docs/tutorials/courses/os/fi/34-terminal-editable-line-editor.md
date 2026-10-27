# Rakenna kohdistinta tukeva komentorivieditori

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Komentotulkki pystyy nyt muokkaamaan rajattua komentoriviä myös keskeltä. Tässä oppitunnissa seurataan PS/2-skannauskoodien reittiä VNT-komentotulkkiin ja käydään läpi lisäys, poisto, Home/End, kohdistimen invariantti sekä QEMU-näppäimistötesti.

Toteutustiedostot: `os/ps2.vnt`, `os/kernel/shell.vnt` ja `.github/workflows/vntos-boot.yml`. Pääte on edelleen tekstipohjainen: VGA-tekstimuisti ja COM1-sarjaportti. Näppäimistöä luetaan pollauksella; tämä muutos ei ota IRQ1-keskeytystä käyttöön.

## Näppäinarvot eivät ole merkkejä

PS/2-ajuri tunnistaa Set 1 -laajennetun näppäimen etuliitteen `0xE0`. Se kuluttaa etuliitteen ja muuntaa seuraavan painalluskoodin sisäiseksi arvoksi:

| Näppäin | Skannauskoodi E0:n jälkeen | Sisäinen arvo |
| --- | --- | ---: |
| Nuoli ylös | `0x48` | 257 |
| Nuoli vasemmalle | `0x4B` | 258 |
| Nuoli oikealle | `0x4D` | 259 |
| Delete | `0x53` | 260 |
| Home | `0x47` | 261 |
| End | `0x4F` | 262 |

Arvot ovat tulostettavien tavumerkkien alueen ulkopuolella. Komentotulkki erottaa navigoinnin ASCII-syötteestä lisäämättä ohjausarvoja komentopuskuriin. Vapautuskoodit ohitetaan. Tuntemattomat laajennetut näppäimet kulutetaan, jotta ne eivät päädy komentoriville.

## Rivieditorin invariantti

Syötepuskuriin mahtuu enintään 127 merkkiä. Aktiivista riviä kuvaavat kaksi arvoa:

- `length`: puskurissa olevien merkkien määrä, lopetusnollaa lukuun ottamatta.
- `cursor`: lisäyskohta, aina nollan ja `length`-arvon välissä.

Keskeinen invariantti on:

```text
0 <= cursor <= length <= 127
buffer[0 .. length-1] = näkyvä komento
buffer[length] = 0
```

Näytön laitteistokohdistimen on vastattava loogista `cursor`-arvoa. Erillinen `console_cursor_left()` siirtää kohdistinta merkkiä poistamatta ja lähettää sarjaporttiin ANSI CSI-left -sekvenssin. Nuoli vasemmalle ja oikealle muuttavat molempia; Home siirtää kohdistimen nollaan ja End rivin loppuun. Editori ei tue monirivisiä komentoja, mielivaltaista näytön vieritystä muokkauksen aikana eikä Unicode-grafeemien mukaista liikkumista.

## Lisää merkki korvaamatta vanhaa

Kun tulostettava merkki saapuu eikä puskuri ole täynnä, siirrä loppuosa yhden paikan oikealle aloittaen lopusta:

```vnt
let shift_index: int = length
while shift_index > cursor {
    buffer[shift_index] = buffer[shift_index - 1]
    shift_index = shift_index - 1
}
buffer[cursor] = character
length = length + 1
buffer[length] = 0
```

Takaperin siirtäminen estää ylikirjoittamasta merkkejä, jotka täytyy vielä siirtää. Komentotulkki piirtää muuttuneen loppuosan, tyhjentää vanhan viimeisen solun, palauttaa näytön kohdistimen lisäyskohtaan ja siirtää sitä sitten yhden merkin eteenpäin.

## Delete ja Backspace ovat eri toimintoja

**Delete** poistaa merkin kohdistimen kohdalta. **Backspace** poistaa välittömästi kohdistinta edeltävän merkin ja siirtää kohdistinta yhden paikan vasemmalle. Molemmat siirtävät jäljellä olevan loppuosan vasemmalle ja lisäävät puskurin lopetusnollan uudelleen.

Apuohjelma `shell_redraw_tail(buffer, cursor, length)` piirtää vain muuttuneen loppuosan. Se tulostaa loppuosan, kirjoittaa välilyönnin vanhan viimeisen merkin poistamiseksi ja kutsuu `console_cursor_left()`-toimintoa, kunnes kohdistin palaa loogiseen lisäyskohtaan. Tämä ei ole sama kuin Backspace: VGA:n Backspace poistaa edellisen solun merkin. Kohdistintoiminto siirtää VGA:n laitteistokohdistinta tekstiä muuttamatta ja lähettää sarjapäätteelle ANSI CSI-left -sekvenssin.

Ratkaisu on tarkoituksella yksinkertainen. Se olettaa, että pääte käsittelee askelpalautuksen kohdistimen liikkeenä ja että komento mahtuu rajattuun puskuriin. Tämä ei ole yleiskäyttöinen ANSI-pääte.

## Historian haku ja vanhan rivin tyhjennys

Nuoli ylös hakee edelleen yhden edellisen ei-tyhjän komennon. Ennen lataamista komentotulkki siirtää kohdistimen rivin alkuun, korvaa vanhat näkyvät merkit välilyönneillä, palauttaa kohdistimen alkuun ja tulostaa haetun komennon. Looginen puskuri, `length` ja `cursor` on päivitettävä yhdessä.

Älä kirjoita koko riviä tyhjentäessäsi toistuvasti välilyöntiä ja heti perään askelpalautusta: tällöin sama solu ylikirjoitetaan. Etene välilyöntejä tulostaessa ja siirry vasta sen jälkeen alkuun kohdistinta liikuttavalla toiminnolla, joka ei poista välilyöntejä. QEMU-testi tarkistaa historian haun navigoinnin jälkeen.

## Automaattinen näppäimistöregressio

Työnkulku `.github/workflows/vntos-boot.yml` syöttää näppäimet QEMUn näppäimistömonitorin kautta. Se tarkistaa nyt kaksi muokkaustapausta ennen historian ja muiden komentojen testaamista:

1. Kirjoita `ech test`, paina Homea, siirry kolme askelta oikealle, lisää `o` ja suorita `echo test`.
2. Kirjoita `echo ttest`, siirry välilyönnin jälkeiseen merkkiin, paina Deletea ja suorita `echo test`.
3. Kirjoita `echo ttest`, paina Endiä, siirry neljä askelta vasemmalle, paina Backspacea ja suorita `echo test`.
4. Paina nuolta ylös ja Enteriä hakeaksesi ja suorittaaksesi edellisen komennon.
5. Suorita `history`, `uptime`, `synctest` sekä nykyiset muistin, IRQ:n, hiiren ja SMP:n diagnostiikkakomennot.

Työnkulun tarkistukset ovat todiste. Lähdekoodimuutos tai uskottava odotettu tuloste ei todista, että kernel kääntyi tai käynnistyi.

## Rajoitukset ja seuraavat vaiheet

- Rivin pituus on enintään 127 merkkiä.
- Historia tallentaa vain yhden komennon.
- Nuoli alas -historiahaku ja komentojen täydennys puuttuvat.
- Sarjaportin syöte käsitellään tavuina; ANSI-escape-jonoja ei tulkita navigointinäppäimiksi.
- Pollaus voi ohittaa tapahtumia, jos toinen polku kuluttaa PS/2-ohjaimen tulostavun. Tuleva keskeytysohjattu ajuri tarvitsee määritellyn syötejonon ja synkronointisopimuksen.
- Komentotulkki ja konsoli eivät ole uudelleenkutsuttavia. Tätä editoria ei saa kutsua rinnakkain AP-CPU:lta tai keskeytyskäsittelijästä.
- Kohdistimen toiminta VGA-rivin lopussa ja näytön vieriessä on testattava QEMUsissa; sarjapäätteet voivat käsitellä ohjausmerkkejä eri tavoin.

Seuraava järkevä askel on erottaa pieni rivieditorin tilakone komentojen käsittelystä ja lisätä rajattu komentotaulukko sekä help-tuloste. Näppäimistökeskeytyksiä ei pidä ottaa käyttöön ennen kuin syötejono, keskeytyksen kuittaus ja jaetun tilan omistajuus on määritelty.

## Harjoitukset

1. Miksi lisäyksessä tavut siirretään oikealta vasemmalle?
2. Miksi Delete ja Backspace muuttavat eri puskurikohtia?
3. Perustele kohdistininvariantti lisäyksen, poiston, Home/End-toimintojen ja historian haun jälkeen.
4. Mitä tapahtuu, jos näytön kohdistinta liikutetaan mutta loogista kohdistinta ei?
5. Laajenna QEMU-testiä keskeltä tehtävällä Backspace-poistolla ja tarkista suoritettu komento.
6. Miksi VGA- ja sarjatulosteen vastaavuus ei ole taattu kaikissa pääte-emulaattoreissa?

## Hyväksymiskriteerit

- [ ] Laajennettuja näppäinarvoja ei lisätä tulostettavina merkkeinä.
- [ ] Nuoli vasemmalle, oikealle, Home, End ja Delete päivittävät kohdistinta ennustettavasti.
- [ ] Lisäys siirtää loppuosan menettämättä merkkejä.
- [ ] Backspace poistaa edeltävän merkin ja Delete nykyisen merkin.
- [ ] Historian haku tyhjentää koko vanhan rivin ja palauttaa loogisen kohdistimen.
- [ ] Puskuri päättyy nollatavuun ja on rajattu 127 merkkiin.
- [ ] CI testaa lisäyksen, Deleten, historian haun ja komentotulosteen QEMUsissa.
- [ ] Suomen- ja englanninkielinen oppitunti kuvaavat samat koodipolut ja rajoitukset.

---

[← Edellinen oppitunti](./33-terminal-history-and-uptime.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./35-terminal-decimal-uptime.md)
