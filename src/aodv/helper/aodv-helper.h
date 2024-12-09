/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * 著作権 (C) 2009 IITP RAS
 *
 * このプログラムはフリーソフトウェアです。再配布や変更ができます。
 * Free Software Foundation によって発表された GNU General Public License バージョン2の条件に従います。
 *
 * このプログラムは役に立つことを期待して配布されていますが、
 * いかなる保証も含まれません。特定の目的への適合性や商品性についての暗黙の保証すらも含まれません。
 * 詳細についてはGNU General Public Licenseをご覧ください。
 *
 * このプログラムは、このプログラムと一緒にGNU General Public Licenseのコピーが提供されるはずです。もし提供されていない場合、Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA に書いてください。
 *
 * 著者: Pavel Boyko <boyko@iitp.ru>、Mathieu Lacage <mathieu.lacage@sophia.inria.fr> の OlsrHelper をもとに書かれました。
 */

#ifndef AODV_HELPER_H
#define AODV_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/ipv4-routing-helper.h"

namespace ns3 {
/**
 * \ingroup aodv
 * \brief ノードに AODV ルーティングを追加するヘルパークラスです。
 */
class AodvHelper : public Ipv4RoutingHelper
{
public:
  AodvHelper ();

  /**
   * この AodvHelper のクローンへのポインタを返します。
   *
   * \internal
   * このメソッドは、主に他のヘルパーによる内部使用のためです。
   * クライアントはこのメソッドによって割り当てられた動的メモリを解放することを期待しています。
   */
  AodvHelper* Copy (void) const;

  /**
   * ルーティングプロトコルを新しく作成して返します。
   *
   * \param node ルーティングプロトコルを実行するノード
   * \returns 新しく作成されたルーティングプロトコルへのポインタ
   *
   * このメソッドは、ns3::InternetStackHelper::Install によって呼び出されます。
   *
   * \todo 利用可能なすべてのIPインターフェースのサブセットにAODVをインストールするサポートを追加
   */
  virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;
  /**
   * 属性の名前と値を設定します。
   *
   * \param name 設定する属性の名前
   * \param value 設定する属性の値
   *
   * このメソッドは ns3::aodv::RoutingProtocol の属性を制御します。
   */
  void Set (std::string name, const AttributeValue &value);
  /**
   * このモデルで使用するランダム変数ストリームに固定のランダム変数ストリーム番号を割り当てます。
   * 割り当てられたストリームの数（おそらくゼロ）が返されます。このヘルパーのインストール()メソッドは、事前にns3::InternetStackHelperによって呼び出されている必要があります。
   *
   * \param stream 使用する最初のストリームインデックス
   * \param c AODVを修正して固定のストリームを使用するようにするノードコンテナ
   * \return このヘルパーによって割り当てられたストリームインデックスの数
   */
  int64_t AssignStreams (NodeContainer c, int64_t stream);

private:
  /** AODV ルーティングオブジェクトを作成するファクトリ。 */
  ObjectFactory m_agentFactory;
};

}

#endif /* AODV_HELPER_H */