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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Selvitä ongelmat järjestelmällisesti](15-debugging.md) · [Seuraava oppitunti: Tee ensimmäinen pieni muutos →](17-first-kernel-change.md)
