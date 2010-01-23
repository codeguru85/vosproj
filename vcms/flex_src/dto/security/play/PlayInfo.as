package dto.security.play
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.tcp.data.auxiliary.PlayInfo")]
	public class PlayInfo
	{
		public var listId:int;
		public var listName:String;
		public var createTime:Date;
		public var playState:String;
		public var videoNum:int;
		public var startDate:Date;
		public var endDate:Date;
		public function PlayInfo()
		{
		}

	}
}