# Testaa toimintaa, älä vain käännöstä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Käännös tarkistaa, että koodi voidaan kääntää. Linkitys tarkistaa symbolit ja muistiasettelun. QEMU-käynnistys tarkistaa, pääseekö kernel käyntiin. Toimintatesti tarkistaa, tapahtuuko odotettu asia. CI ajaa tarkistuksia automaattisesti palvelimella.

## Sanasto

Ennakkoehto on testin tarvitsema lähtötila. Odotettu tulos määrittää onnistumisen. Regressiotesti estää tunnetun virheen palaamisen. CI tarkoittaa jatkuvan integraation automaattisia tarkistuksia.

## Käytännön harjoitus

Määritä ennen muutosta testin syöte, odotettu tulos ja se, miten epäonnistuminen havaitaan. Keskeytystestissä käynnistyminen ei riitä; tarvitaan merkki käsittelijän suorituksesta. Muistitestissä pitää tarkistaa, ettei kehystä varata kahdesti.

## Yleinen virhe ja turvallisuus

Testi, joka vain tulostaa OK tarkistamatta väitettä, ei todista toimintaa. Älä väitä CI:tä onnistuneeksi, jos ajo on vielä kesken tai epäonnistunut.

## Tarkista ymmärtäminen

Valitse ominaisuus ja määritä ennakkoehto, toiminto, odotettu tulos ja epäonnistumisen tunnistus.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä testi epäonnistuu, kun ominaisuus on rikki. Mieti negatiivista tapausta: laite ei vastaa, syöte on väärä tai vapaa muisti loppuu. Myös epäonnistumisen pitää olla hallittu ja havaittava.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Selvitä ongelmat järjestelmällisesti](15-debugging.md) · [Seuraava oppitunti: Tee ensimmäinen pieni muutos →](17-first-kernel-change.md)
