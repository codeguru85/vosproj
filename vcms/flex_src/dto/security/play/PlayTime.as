package dto.security.play
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.tcp.data.auxiliary.PlayTime")]
	public class PlayTime
	{
		public var videoId:int;
		public var playVideoName:String;
		public var playTime:String;
		public var playFilePath:String;
		public var timeCount:String;
		public function PlayTime()
		{
		}

	}
}